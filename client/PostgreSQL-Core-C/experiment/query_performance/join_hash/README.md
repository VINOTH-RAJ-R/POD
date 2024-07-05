# General Experiment Scripts

## Table Schema

Place your table schemas at ``./schemas/``. ``schema.sql`` is the schema for base type database. ``schemaa.sql`` is the schema for marked type database. ``schema_not_null.sql`` is only used for generating values. It should be consistant to ``schema.sql``, but with all fields tagged *NOT NULL* constriants.

## Experiment Configuration

``./exp_config.sh`` configures the experiment. You can set

- experiment title
- database sizes
- null rates
- is the constants/nulls marked
- how many times the queries run

## Queries

Place your query sql files at ``./queries/``. The experiment runs the queries in ``init.sql`` first. And then it benchmarks the queries in ``marked.sql`` in marked databases and ``notmarked.sql`` in not marked databases (if enabled).
