# pawn-plus-mysql

[![sampctl](https://shields.southcla.ws/badge/sampctl-pawn--async--mysql-2f2f2f.svg?style=for-the-badge)](https://github.com/AGraber/pawn-plus-mysql)

PawnPlus extensions and helpers for the MySQL plugin

## Installation

Simply install to your project:

```bash
sampctl package install AGraber/pawn-plus-mysql
```

Include in your code and begin using the library:

```pawn
#include <pp-mysql>
```

It is recommended that you set a PawnPlus version explicitely on your pawn.json
(preferibly the latest) to avoid always downloading the latest one.

If you don't use sampctl, just download the `pp-mysql.inc` include and
drop it to your `includes/` folder, and then download the PawnPlus plugin and
include [from here](https://github.com/IllidanS4/PawnPlus/releases).

While you're on it and if you don't use PawnPlus yet,
[you should check it out!](https://github.com/IllidanS4/PawnPlus/blob/master/README.md)

## Usage

All existing `mysql_[t/p]query` now have a variant that accepts 
PawnPlus strings, ending with `_s` (for example, `mysql_tquery_s`).

In addition, a new member arrives into the family of these
natives: `mysql_aquery[_s]`, which returns a task that will be
set as completed when the query completes successfully, or
fault when there's an error during its execution.

Note that `mysql_aquery[_s]` will piggyback on `mysql_tquery` by default. You
can override this and use `mysql_tquery` by setting the `parallel` parameter at
the end to `true`, or explicitly change this default value by defining
`MYSQL_ASYNC_DEFAULT_PARALLEL` to you desired value.

## Examples

Using PawnPlus strings on natives

```pawn
public OnPlayerTookAllCandies(playerid)
{
	mysql_tquery_s(DatabaseHandle, @("UPDATE player SET took_candies = UNIX_TIMESTAMP() WHERE id = ") % GetPlayerDBID(playerid));
}
```

Task-based asynchronous queries:
```pawn
LoadPlayerData(playerid)
{
	await mysql_aquery_s(DatabaseHandle, @("SELECT * FROM player WHERE id = ) % GetPlayerDBID(playerid), .parallel = false);
	// Cache functions will be available after this
	cache_get_value_name_float(0, "health", PlayerHealth[playerid]);
	// ...
}
```
