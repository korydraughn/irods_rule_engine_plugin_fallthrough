# iRODS Rule Engine Plugin - Fallthrough

This plugin was developed to test fallthrough support in [iRODS](https://github.com/irods/irods).

## Requirements

- irods-externals packages
- irods-dev package
- irods-runtime package

## Compiling

```bash
$ git clone https://github.com/irods/irods_rule_engine_plugin_update_collection_mtime
$ mkdir _build
$ cd _build
$ cmake -GNinja ../irods_rule_engine_plugin_update_collection_mtime
$ ninja package
```
You should now have a `deb` package with a name similar to the following:

```bash
irods-rule-engine-plugin-update-collection-mtime-<plugin_version>-<os>-<arch>.deb
```

## Installing

```bash
$ sudo dpkg -i irods-rule-engine-plugin-update-collection-mtime-<plugin_version>-<os>-<arch>.deb
```

## Configuration

To enable, prepend the following plugin config to the `rule_engines` list in `/etc/irods/server_config.json`.

```javascript
"rule_engines": [
    {
        "instance_name": "irods_rule_engine_plugin-fallthrough-instance",
        "plugin_name": "irods_rule_engine_plugin-fallthrough",
        "plugin_specific_configuration": {}
    },
    
    // ... Previously installed rule engine plugin configs ...
```

