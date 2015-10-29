# MiniSQL
DBMS project.

## Dependencies
Ubuntu
```sh
apt-get install readline-dev
```

Fedora
```sh
yum install readline-devel
```

ArchLinux
```sh
pacman -S readline
```

## Compile
Install latest version of `clang++` and `GNUMake`.
```sh
./bootstrap
```

## Usage

```sh
minisql [script] [--help]
```

If `--help` is provided, a document will be shown.

If `script` is provided, which is a valid filepath, the SQL statements in this script will be executed in order.

If nothing is provided, REPL mode will be ready, you can use SQL statement in it.
