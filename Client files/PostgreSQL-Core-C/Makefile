MODULES = src/markednull src/intt src/datee src/textt src/numericc
EXTENSION = markednullcore
DATA_built = markednullcore--0.1.sql

markednullcore--0.1.sql: src/intt.sql src/datee.sql src/textt.sql src/numericc.sql
	cat $^ > $@
	#sed -i "s|<NOW>|$(shell date "+%D %T")|g" $@

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
