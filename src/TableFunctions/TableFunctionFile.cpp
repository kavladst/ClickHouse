#include <TableFunctions/TableFunctionFile.h>
#include <TableFunctions/parseColumnsListForTableFunction.h>

#include "Parsers/IAST_fwd.h"
#include "registerTableFunctions.h"
#include <Access/Common/AccessFlags.h>
#include <Interpreters/Context.h>
#include <Storages/ColumnsDescription.h>
#include <Storages/StorageFile.h>
#include <TableFunctions/TableFunctionFactory.h>
#include <Interpreters/evaluateConstantExpression.h>
#include <Formats/FormatFactory.h>
#include <Parsers/ASTIdentifier_fwd.h>

namespace DB
{

namespace ErrorCodes
{
    extern const int LOGICAL_ERROR;
    extern const int BAD_ARGUMENTS;
}

void TableFunctionFile::parseFirstArguments(const ASTPtr & arg, ContextPtr context)
{
    if (context->getApplicationType() != Context::ApplicationType::LOCAL)
    {
        ITableFunctionFileLike::parseFirstArguments(arg, context);
        return;
    }

    if (auto opt_name = tryGetIdentifierName(arg))
    {
        if (*opt_name == "stdin")
            fd = STDIN_FILENO;
        else if (*opt_name == "stdout")
            fd = STDOUT_FILENO;
        else if (*opt_name == "stderr")
            fd = STDERR_FILENO;
        else
            filename = *opt_name;
    }
    else if (const auto * literal = arg->as<ASTLiteral>())
    {
        auto type = literal->value.getType();
        if (type == Field::Types::Int64 || type == Field::Types::UInt64)
        {
            fd = (type == Field::Types::Int64) ? static_cast<int>(literal->value.get<Int64>()) : static_cast<int>(literal->value.get<UInt64>());
            if (fd < 0)
                throw Exception("File descriptor must be non-negative", ErrorCodes::BAD_ARGUMENTS);
        }
        else if (type == Field::Types::String)
        {
            filename = literal->value.get<String>();
            if (filename == "-")
                fd = STDIN_FILENO;
        }
        else
            throw Exception(
                "The first argument of table function '" + getName() + "' mush be path or file descriptor", ErrorCodes::BAD_ARGUMENTS);
    }
}

String TableFunctionFile::getFormatFromFirstArgument()
{
    if (fd >= 0)
        return FormatFactory::instance().getFormatFromFileDescriptor(fd);
    else
        return FormatFactory::instance().getFormatFromFileName(filename, true);
}

StoragePtr TableFunctionFile::getStorage(const String & source,
    const String & format_, const ColumnsDescription & columns,
    ContextPtr global_context, const std::string & table_name,
    const std::string & compression_method_) const
{
    // For `file` table function, we are going to use format settings from the
    // query context.
    StorageFile::CommonArguments args{
        WithContext(global_context),
        StorageID(getDatabaseName(), table_name),
        format_,
        std::nullopt /*format settings*/,
        compression_method_,
        columns,
        ConstraintsDescription{},
        String{},
    };
    if (fd >= 0)
        return StorageFile::create(fd, args);

    return StorageFile::create(source, global_context->getUserFilesPath(), args);
}

ColumnsDescription TableFunctionFile::getActualTableStructure(ContextPtr context) const
{
    if (structure == "auto")
    {
        if (fd >= 0)
            throw Exception(
                "Schema inference is not supported for table function '" + getName() + "' with file descriptor", ErrorCodes::LOGICAL_ERROR);
        size_t total_bytes_to_read = 0;
        Strings paths = StorageFile::getPathsList(filename, context->getUserFilesPath(), context, total_bytes_to_read);
        return StorageFile::getTableStructureFromFile(format, paths, compression_method, std::nullopt, context);
    }


    return parseColumnsListFromString(structure, context);
}

void registerTableFunctionFile(TableFunctionFactory & factory)
{
    factory.registerFunction<TableFunctionFile>();
}

}
