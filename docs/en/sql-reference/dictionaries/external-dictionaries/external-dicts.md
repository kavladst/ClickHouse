---
sidebar_position: 39
sidebar_label: General Description
---

# External Dictionaries 

You can add your own dictionaries from various data sources. The data source for a dictionary can be a local text or executable file, an HTTP(s) resource, or another DBMS. For more information, see “[Sources for external dictionaries](../../../sql-reference/dictionaries/external-dictionaries/external-dicts-dict-sources.md)”.

ClickHouse:

-   Fully or partially stores dictionaries in RAM.
-   Periodically updates dictionaries and dynamically loads missing values. In other words, dictionaries can be loaded dynamically.
-   Allows to create external dictionaries with xml files or [DDL queries](../../../sql-reference/statements/create/dictionary.md).

The configuration of external dictionaries can be located in one or more xml-files. The path to the configuration is specified in the [dictionaries_config](../../../operations/server-configuration-parameters/settings.md#server_configuration_parameters-dictionaries_config) parameter.

Dictionaries can be loaded at server startup or at first use, depending on the [dictionaries_lazy_load](../../../operations/server-configuration-parameters/settings.md#server_configuration_parameters-dictionaries_lazy_load) setting.

The [dictionaries](../../../operations/system-tables/dictionaries.md#system_tables-dictionaries) system table contains information about dictionaries configured at server. For each dictionary you can find there:

-   Status of the dictionary.
-   Configuration parameters.
-   Metrics like amount of RAM allocated for the dictionary or a number of queries since the dictionary was successfully loaded.

The dictionary configuration file has the following format:

``` xml
<clickhouse>
    <comment>An optional element with any content. Ignored by the ClickHouse server.</comment>

    <!--Optional element. File name with substitutions-->
    <include_from>/etc/metrika.xml</include_from>


    <dictionary>
        <!-- Dictionary configuration. -->
        <!-- There can be any number of <dictionary> sections in the configuration file. -->
    </dictionary>

</clickhouse>
```

You can [configure](../../../sql-reference/dictionaries/external-dictionaries/external-dicts-dict.md) any number of dictionaries in the same file.

[DDL queries for dictionaries](../../../sql-reference/statements/create/dictionary.md) does not require any additional records in server configuration. They allow to work with dictionaries as first-class entities, like tables or views.

:::note    
You can convert values for a small dictionary by describing it in a `SELECT` query (see the [transform](../../../sql-reference/functions/other-functions.md) function). This functionality is not related to external dictionaries.
:::

## See Also {#ext-dicts-see-also}

-   [Configuring an External Dictionary](../../../sql-reference/dictionaries/external-dictionaries/external-dicts-dict.md)
-   [Storing Dictionaries in Memory](../../../sql-reference/dictionaries/external-dictionaries/external-dicts-dict-layout.md)
-   [Dictionary Updates](../../../sql-reference/dictionaries/external-dictionaries/external-dicts-dict-lifetime.md)
-   [Sources of External Dictionaries](../../../sql-reference/dictionaries/external-dictionaries/external-dicts-dict-sources.md)
-   [Dictionary Key and Fields](../../../sql-reference/dictionaries/external-dictionaries/external-dicts-dict-structure.md)
-   [Functions for Working with External Dictionaries](../../../sql-reference/functions/ext-dict-functions.md)

