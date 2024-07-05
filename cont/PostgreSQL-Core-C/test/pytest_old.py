# Use pytest

import psycopg2
import configparser

db_config = configparser.ConfigParser()
db_config.read("test.config")
pgsql = db_config["pgsql"]

conn = psycopg2.connect(
    dbname=pgsql["dbname"],
    user=pgsql['user'],
    password=pgsql['password'],
    host=pgsql['host'],
    port=pgsql['port']
)


def query(sql, fetch=0):
    """
    Execute a SQL statement fetch (or ignore) the result.
    :param sql: the SQL statement of the query
    :param fetch: 0 for do not fetch, 1 for fetch the first result, 2 for fetch all the results
    :return: a list [success_flag, results] success_flag: 1 for success, -1 for not; results: a list of records
    """
    cur = conn.cursor()
    to_return = [1, None]
    try:
        cur.execute(sql)
        if fetch == 1:
            to_return[1] = cur.fetchone()[0]
        elif fetch == 2:
            to_return[1] = cur.fetchall()
    except:
        to_return[0] = -1
        conn.rollback()
    finally:
        cur.close()
        conn.commit()
    return to_return


class TestMarkedNull:

    class TestIntt:

        class TestIO:

            def test_constant(self):
                sql = "SELECT '123'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 123

                sql = "SELECT '0'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '-123'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == -123

            def test_constant_illegal(self):
                sql = "SELECT ''::intt;"
                flag, result = query(sql, 1)
                assert flag == -1

                sql = "SELECT 'abcde'::intt;"
                flag, result = query(sql, 1)
                assert flag == -1

                sql = "SELECT '123.456'::intt;"
                flag, result = query(sql, 1)
                assert flag == -1

                sql = "SELECT '123abcba321'::intt;"
                flag, result = query(sql, 1)
                assert flag == -1

            def test_markednull(self):
                sql = "SELECT 'nUlL:123'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result == 'NULL:123'

            def test_markednull_illegal(self):
                sql = "SELECT 'null:0'::intt;"
                flag, result = query(sql, 1)
                assert flag == -1

                sql = "SELECT 'null:-1'::intt;"
                flag, result = query(sql, 1)
                assert flag == -1

                sql = "SELECT 'null:abcde'::intt;"
                flag, result = query(sql, 1)
                assert flag == -1

                sql = "SELECT 'null:abcde'::intt;"
                flag, result = query(sql, 1)
                assert flag == -1

                sql = "SELECT 'null:'::intt;"
                flag, result = query(sql, 1)
                assert flag == -1

            def test_sqlnull(self):
                sql = "SELECT NULL::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

        class TestCast:

            def test_from_base(self):
                sql = "SELECT 123::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 123

                sql = "SELECT 0::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT -123::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == -123

            def test_to_base(self):
                sql = "SELECT '123'::intt::int;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 123

                sql = "SELECT 'null:123'::intt::int;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

        class TestMathOpt:

            def test_add(self):
                sql = "SELECT + '300'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 300

                sql = "SELECT '100'::intt + '200'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 300

                sql = "SELECT '100'::int + '200'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 300

                sql = "SELECT '100'::intt + '200'::int;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 300

                sql = "SELECT 'null:100'::intt + '200'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT '100'::intt + 'null:200'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

            def test_minus(self):
                sql = "SELECT - '100'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == -100

                sql = "SELECT '300'::intt - '200'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 100

                sql = "SELECT '300'::int - '200'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 100

                sql = "SELECT '300'::intt - '200'::int;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 100

                sql = "SELECT 'null:300'::intt - '200'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT '300'::intt - 'null:200'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

            def test_multiply(self):
                sql = "SELECT '100'::intt * '200'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 20000

                sql = "SELECT '100'::int * '200'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 20000

                sql = "SELECT '100'::intt * '200'::int;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 20000

                sql = "SELECT 'null:100'::intt * '200'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT '100'::intt * 'null:200'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

            def test_divide(self):
                sql = "SELECT '200'::intt / '100'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 2

                sql = "SELECT '200'::int / '100'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 2

                sql = "SELECT '200'::intt / '100'::int;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 2

                sql = "SELECT '200'::intt / '0'::intt;"
                flag, result = query(sql, 1)
                assert flag == -1

                sql = "SELECT 'null:200'::intt / '100'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT '200'::intt / 'null:100'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

            def test_modulo(self):
                sql = "SELECT '108'::intt % '10'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 8

                sql = "SELECT '108'::int % '10'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 8

                sql = "SELECT '108'::intt % '10'::int;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 8

                sql = "SELECT 'null:108'::intt % '10'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT '108'::intt % 'null:10'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

            def test_exponential(self):
                sql = "SELECT '2'::intt ^ '3'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert float(result) == 8

                sql = "SELECT '2'::int ^ '3'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert float(result) == 8

                sql = "SELECT '2'::intt ^ '3'::int;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert float(result) == 8

                sql = "SELECT 'null:2'::intt ^ '3'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT '2'::intt ^ 'null:3'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

            def test_absolute(self):
                sql = "SELECT @'-8'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 8

                sql = "SELECT @'0'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT @'null:8'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

            def test_bitwise_and(self):
                sql = "SELECT '152'::intt & '25'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 24

                sql = "SELECT '152'::int & '25'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 24

                sql = "SELECT '152'::intt & '25'::int;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 24

                sql = "SELECT 'null:152'::intt & '25'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT '152'::intt & 'null:25'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

            def test_bitwise_or(self):
                sql = "SELECT '16'::intt | '8'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 24

                sql = "SELECT '16'::int | '8'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 24

                sql = "SELECT '16'::intt | '8'::int;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 24

                sql = "SELECT 'null:16'::intt | '8'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT '16'::intt | 'null:8'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

            def test_bitwise_xor(self):
                sql = "SELECT '83'::intt # '75'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 24

                sql = "SELECT '83'::int # '75'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 24

                sql = "SELECT '83'::intt # '75'::int;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 24

                sql = "SELECT 'null:83'::intt # '75'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT '83'::intt # 'null:75'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

            def test_bitwise_not(self):
                sql = "SELECT ~'-25'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 24

                sql = "SELECT ~'null:25'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

            def test_bitwise_shift_left(self):
                sql = "SELECT '3'::intt << '3'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 24

                sql = "SELECT '3'::int << '3'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 24

                sql = "SELECT '3'::intt << '3'::int;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 24

                sql = "SELECT 'null:3'::intt # '3'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT '3'::intt # 'null:3'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

            def test_bitwise_shift_right(self):
                sql = "SELECT '192'::intt >> '3'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 24

                sql = "SELECT '192'::int >> '3'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 24

                sql = "SELECT '192'::intt >> '3'::int;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 24

                sql = "SELECT 'null:192'::intt # '3'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT '192'::intt # 'null:3'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

        class TestCprOpt:

            def test_gt(self):
                sql = "SELECT '3'::intt > '4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '3'::intt > '3'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '3'::intt > '2'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3'::intt > 'null:4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) is 1

                sql = "SELECT 'null:3'::intt > '4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) is 0

                sql = "SELECT 'null:3'::intt > 'null:4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) is 0

                sql = "SELECT '3'::intt |> 'null:4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result == None

                sql = "SELECT 'null:3'::intt |> '4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result == None

                sql = "SELECT 'null:3'::intt |> 'null:4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result == None

                sql = "SELECT 'null:3'::intt |> 'null:3'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT 'null:3'::intt |> 'null:2'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result == None

            def test_ge(self):
                sql = "SELECT '3'::intt >= '4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '3'::intt >= '3'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3'::intt >= '2'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3'::intt >= 'null:4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT 'null:3'::intt >= '4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT 'null:3'::intt >= 'null:4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT 'null:3'::intt >= 'null:3'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3'::intt |>= 'null:4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT 'null:3'::intt |>= '4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT 'null:3'::intt |>= 'null:4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT 'null:3'::intt |>= 'null:3'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT 'null:3'::intt |>= 'null:2'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

            def test_lt(self):
                sql = "SELECT '3'::intt < '4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3'::intt < '3'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '3'::intt < '2'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '3'::intt < 'null:4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT 'null:3'::intt < '4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT 'null:3'::intt < 'null:4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3'::intt |< 'null:4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT 'null:3'::intt |< '4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT 'null:3'::intt |< 'null:4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT 'null:3'::intt |< 'null:3'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT 'null:3'::intt |< 'null:2'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

            def test_le(self):
                sql = "SELECT '3'::intt <= '4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3'::intt <= '3'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3'::intt <= '2'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '3'::intt <= 'null:4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT 'null:3'::intt <= '4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT 'null:3'::intt <= 'null:4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT 'null:3'::intt <= 'null:3'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3'::intt |<= 'null:4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT 'null:3'::intt |<= '4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT 'null:3'::intt |<= 'null:4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT 'null:3'::intt |<= 'null:3'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT 'null:3'::intt |<= 'null:2'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

            def test_eq(self):
                sql = "SELECT '3'::intt = '4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '3'::intt = '3'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3'::intt = '2'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '3'::intt = 'null:4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT 'null:3'::intt = '4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT 'null:3'::intt = 'null:4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT 'null:3'::intt = 'null:3'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3'::intt |= 'null:4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT 'null:3'::intt |= '4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT 'null:3'::intt |= 'null:4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT 'null:3'::intt |= 'null:3'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT 'null:3'::intt |= 'null:2'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

            def test_ne(self):
                sql = "SELECT '3'::intt <> '4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3'::intt <> '3'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '3'::intt <> '2'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3'::intt <> 'null:4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT 'null:3'::intt <> '4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT 'null:3'::intt <> 'null:4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT 'null:3'::intt <> 'null:3'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '3'::intt |<> 'null:4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT 'null:3'::intt |<> '4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT 'null:3'::intt |<> 'null:4'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT 'null:3'::intt |<> 'null:3'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT 'null:3'::intt |<> 'null:2'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

        class TestIdxAndAggr:

            def test_init(self):
                sql = '''
                    DROP TABLE IF EXISTS r;
                    CREATE TABLE r (i intt);
                    INSERT INTO r VALUES (NULL), ('null:300'), (NULL), (500), ('null:500'),
                                         (200), ('null:200'), ('null:400'), (NULL), (NULL),
                                         (300), (100), ('null:100'), (400), (NULL);
                '''
                flag, result = query(sql, 0)
                assert flag == 1

            def test_btree_index(self):
                sql = "CREATE INDEX r_index ON r USING btree(i mn_intt_ops);"
                flag, result = query(sql, 0)
                assert flag == 1
                sql = "SELECT t.* FROM r t;"
                flag, result = query(sql, 2)
                assert flag == 1
                assert len(result) == 15

            def test_order_by(self):
                sql = "SELECT * FROM r ORDER BY i;"
                flag, result = query(sql, 2)
                assert flag == 1
                assert result == [('NULL:100',), ('NULL:200',), ('NULL:300',), ('NULL:400',), ('NULL:500',),
                                  ('100',), ('200',), ('300',), ('400',), ('500',),
                                  (None,), (None,), (None,), (None,), (None,)]

            def test_is_const(self):
                sql = "SELECT * FROM r WHERE is_const(i);"
                flag, result = query(sql, 2)
                assert flag == 1
                assert len(result) == 5

            def test_is_marked_null(self):
                sql = "SELECT * FROM r WHERE is_marked_null(i);"
                flag, result = query(sql, 2)
                assert flag == 1
                assert len(result) == 5

            def test_sum(self):
                sql = "SELECT sum(i) FROM r;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1500

            def test_max(self):
                sql = "SELECT max(i) FROM r WHERE is_const(i);"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 500

            def test_min(self):
                sql = "SELECT min(i) FROM r WHERE is_const(i);"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 100

            def test_avg(self):
                sql = "SELECT avg(i) FROM r;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 300

        class TestHash:

            def test_init(self):
                sql = '''
                    DROP TABLE IF EXISTS rr;
                    CREATE TABLE rr (a intt, b intt);
                    INSERT INTO rr VALUES
                        (1, 100), (1, 200), (1, 300), (1, 400),
                        (2, 100), (2, 200), (2, 300),
                        (3, 100);
                '''
                flag, result = query(sql, 0)
                assert flag == 1

            def test_hash_results(self):
                sql = 'SELECT a, sum(b) FROM rr GROUP BY a ORDER BY a;'
                flag, result = query(sql, 2)
                assert flag == 1
                assert result == [('1', 1000,), ('2', 600,), ('3', 100)]

            def test_hash_execution_plan(self):
                sql = 'EXPLAIN SELECT a, sum(b) FROM rr GROUP BY a;'
                flag, result = query(sql, 1)
                assert flag == 1
                assert 'HashAggregate' in result

        class TestPartialAggr:

            def test_init(self):
                sql = '''
                    DROP TABLE IF EXISTS rrr;
                    CREATE TABLE rrr (a intt, b intt);
                '''
                query(sql, 0)
                sql = 'INSERT INTO rrr VALUES '
                sql_part = '(1, 1), (2, 2), (3, \'null:3\'), (4, NULL), '
                for i in range(150000):
                    sql += sql_part
                sql += '(0, -500), (5, 500);'
                flag, result = query(sql, 0)
                assert flag == 1

            def test_sum(self):
                sql = 'SELECT sum(b) from rrr;'
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 450000

            def test_sum_group_by(self):
                sql = 'SELECT a, sum(rrr.b) from rrr group by a order by a;'
                flag, results = query(sql, 2)
                assert flag == 1
                assert results == [('0', -500), ('1', 150000), ('2', 300000), ('3', None), ('4', None), ('5', 500)]

            def test_max(self):
                sql = 'SELECT max(b) from rrr;'
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 500

            def test_min(self):
                sql = 'SELECT min(b) from rrr;'
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == -500

            def test_avg(self):
                sql = 'SELECT avg(b) from rrr;'
                flag, result = query(sql, 1)
                assert flag == 1
                assert float(result) - 1.5 < 0.00001

            def test_avg_group_by(self):
                sql = 'SELECT a, avg(rrr.b)::int from rrr group by a order by a;'
                flag, results = query(sql, 2)
                assert flag == 1
                assert results == [('0', -500), ('1', 1), ('2', 2), ('3', None), ('4', None), ('5', 500)]

    class TestTextt:

        class TestIO:

            def test_constant(self):
                sql = "SELECT 'abcdefg'::textt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert str(result) == 'abcdefg'

                sql = "SELECT ''::textt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert str(result) == ''

                sql = "SELECT '"
                long_str = ""
                for i in range(1024 * 1024):
                    long_str += "abc"
                sql += long_str
                sql += "'::textt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert str(result) == long_str

            def test_markednull(self):
                sql = "SELECT 'nUlL:123'::textt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result == 'NULL:123'

            def test_markednull_illegal(self):
                sql = "SELECT 'null:0'::textt;"
                flag, result = query(sql, 1)
                assert flag == -1

                sql = "SELECT 'null:-1'::textt;"
                flag, result = query(sql, 1)
                assert flag == -1

                sql = "SELECT 'null:abcde'::textt;"
                flag, result = query(sql, 1)
                assert flag == -1

                sql = "SELECT 'null:abcde'::textt;"
                flag, result = query(sql, 1)
                assert flag == -1

                sql = "SELECT 'null:'::textt;"
                flag, result = query(sql, 1)
                assert flag == -1

            def test_sqlnull(self):
                sql = "SELECT NULL::textt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

        class TestCast:

            def test_from_base(self):
                sql = "SELECT 'abcde'::text::textt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert str(result) == 'abcde'

                sql = "SELECT ''::text::textt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert str(result) == ''

            def test_to_base(self):
                sql = "SELECT 'abcde'::textt::text;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert str(result) == 'abcde'

                sql = "SELECT 'null:123'::textt::text;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

        class TestStrOpt:

            def test_str_concat(self):
                sql = "SELECT 'abc'::textt || 'def'::textt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert str(result) == 'abcdef'

            def test_str_like(self):
                sql = "SELECT 'abc'::textt LIKE '%b%'::textt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is True

            def test_str_not_like(self):
                sql = "SELECT 'abc'::textt NOT LIKE '%b%'::textt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is False

        class TestCprOpt:

            def test_gt(self):
                sql = "SELECT '3'::textt > '4'::textt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is False

                sql = "SELECT '3'::textt > '3'::textt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is False

                sql = "SELECT '3'::textt > '2'::textt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is True

            def test_ge(self):
                sql = "SELECT '3'::textt >= '4'::textt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is False

                sql = "SELECT '3'::textt >= '3'::textt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is True

                sql = "SELECT '3'::textt >= '2'::textt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is True

            def test_lt(self):
                sql = "SELECT '3'::textt < '4'::textt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is True

                sql = "SELECT '3'::textt < '3'::textt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is False

                sql = "SELECT '3'::textt < '2'::textt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is False

            def test_le(self):
                sql = "SELECT '3'::textt <= '4'::textt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is True

                sql = "SELECT '3'::textt <= '3'::textt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is True

                sql = "SELECT '3'::textt <= '2'::textt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is False

            def test_eq(self):
                sql = "SELECT '3'::textt = '4'::textt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is False

                sql = "SELECT '3'::textt = '3'::textt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is True

                sql = "SELECT '3'::textt = '2'::textt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is False

            def test_ne(self):
                sql = "SELECT '3'::textt != '4'::textt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is True

                sql = "SELECT '3'::textt != '3'::textt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is False

                sql = "SELECT '3'::textt != '2'::textt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is True

        class TestIdxAndAggr:

            def test_init(self):
                sql = '''
                    DROP TABLE IF EXISTS r;
                    CREATE TABLE r (t textt);
                    INSERT INTO r VALUES
                    (NULL), ('null:300'), (NULL), ('EEEEE'), ('null:500'),
                    ('BBBBB'), ('null:200'), ('null:400'), (NULL), (NULL),
                    ('CCCCC'), ('AAAAA'), ('null:100'), ('DDDDD'), (NULL);
                '''
                flag, result = query(sql, 0)
                assert flag == 1

            def test_btree_index(self):
                sql = "CREATE INDEX r_index ON r USING btree(t mn_textt_ops);"
                flag, result = query(sql, 0)
                assert flag == 1
                sql = "SELECT r.* FROM r r;"
                flag, result = query(sql, 2)
                assert flag == 1
                assert len(result) == 15

            def test_order_by(self):
                sql = "SELECT * FROM r ORDER BY t;"
                flag, result = query(sql, 2)
                assert flag == 1
                assert result == [('NULL:100',), ('NULL:200',), ('NULL:300',), ('NULL:400',), ('NULL:500',),
                                  ('AAAAA',), ('BBBBB',), ('CCCCC',), ('DDDDD',), ('EEEEE',),
                                  (None,), (None,), (None,), (None,), (None,)]

            def test_is_const(self):
                sql = "SELECT * FROM r WHERE is_const(t);"
                flag, result = query(sql, 2)
                assert flag == 1
                assert len(result) == 5

            def test_is_marked_null(self):
                sql = "SELECT * FROM r WHERE is_marked_null(t);"
                flag, result = query(sql, 2)
                assert flag == 1
                assert len(result) == 5

            def test_max(self):
                sql = "SELECT max(t) FROM r WHERE is_const(t);"
                flag, result = query(sql, 1)
                assert flag == 1
                assert str(result) == 'EEEEE'

            def test_min(self):
                sql = "SELECT min(t) FROM r WHERE is_const(t);"
                flag, result = query(sql, 1)
                assert flag == 1
                assert str(result) == 'AAAAA'

        class TestHash:

            def test_init(self):
                sql = '''
                    DROP TABLE IF EXISTS rr;
                    CREATE TABLE rr (a textt, b intt);
                    INSERT INTO rr VALUES
                        ('a', 100), ('a', 200), ('a', 300), ('a', 400),
                        ('b', 100), ('b', 200), ('b', 300),
                        ('c', 100);
                '''
                flag, result = query(sql, 0)
                assert flag == 1

            def test_hash_results(self):
                sql = 'SELECT a, sum(b) FROM rr GROUP BY a ORDER BY a;'
                flag, result = query(sql, 2)
                assert flag == 1
                assert result == [('a', 1000,), ('b', 600,), ('c', 100)]

            def test_hash_execution_plan(self):
                sql = 'EXPLAIN SELECT a, sum(b) FROM rr GROUP BY a;'
                flag, result = query(sql, 1)
                assert flag == 1
                assert 'HashAggregate' in result

    class TestDatee:

        class TestIO:

            def test_constant(self):
                sql = "SELECT '2021-09-23'::datee;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert str(result) == '2021-09-23'

                sql = "SELECT '4713-01-01 BC'::datee;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert str(result) == '4713-01-01 BC'

            def test_constant_illegal(self):
                sql = "SELECT ''::datee;"
                flag, result = query(sql, 1)
                assert flag == -1

                sql = "SELECT 'abcde'::datee;"
                flag, result = query(sql, 1)
                assert flag == -1

                sql = "SELECT '123456'::datee;"
                flag, result = query(sql, 1)
                assert flag == -1

                sql = "SELECT '2021-02-31'::datee;"
                flag, result = query(sql, 1)
                assert flag == -1

                sql = "SELECT '2021-13-05'::datee;"
                flag, result = query(sql, 1)
                assert flag == -1

            def test_markednull(self):
                sql = "SELECT 'nUlL:123'::datee;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result == 'NULL:123'

            def test_markednull_illegal(self):
                sql = "SELECT 'null:0'::datee;"
                flag, result = query(sql, 1)
                assert flag == -1

                sql = "SELECT 'null:-1'::datee;"
                flag, result = query(sql, 1)
                assert flag == -1

                sql = "SELECT 'null:abcde'::datee;"
                flag, result = query(sql, 1)
                assert flag == -1

                sql = "SELECT 'null:abcde'::datee;"
                flag, result = query(sql, 1)
                assert flag == -1

                sql = "SELECT 'null:'::datee;"
                flag, result = query(sql, 1)
                assert flag == -1

            def test_sqlnull(self):
                sql = "SELECT NULL::datee;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

        class TestCast:

            def test_from_base(self):
                sql = "SELECT '2021-09-23'::date::datee;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert str(result) == '2021-09-23'

                sql = "SELECT '4713-01-01 BC'::date::datee;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert str(result) == '4713-01-01 BC'

            def test_to_base(self):
                sql = "SELECT '2021-09-23'::datee::date;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert str(result) == '2021-09-23'

                sql = "SELECT 'null:123'::datee::date;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

        class TestMathOpt:

            def test_add(self):
                sql = "SELECT '2021-01-01'::datee + 30;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert str(result) == '2021-01-31'

                sql = "SELECT 30 + '2021-01-01'::datee;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert str(result) == '2021-01-31'

                sql = "SELECT 'null:100'::datee + 30;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT 30 + 'null:100'::datee;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT '2021-01-01'::datee + 'null:100'::intt;"
                flag, result = query(sql, 1)
                assert flag == -1

                sql = "SELECT 'null:100'::intt + '2021-01-01'::datee;"
                flag, result = query(sql, 1)
                assert flag == -1

            def test_minus(self):
                sql = "SELECT '2021-01-31'::datee - 30;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert str(result) == '2021-01-01'

                sql = "SELECT 'null:100'::datee + 30;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

            def test_difference(self):
                sql = "SELECT '2021-01-31'::datee - '2021-01-01'::datee;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 30

                sql = "SELECT 'null:100'::datee - '2021-01-01'::datee;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

        class TestCprOpt:

            def test_gt(self):
                sql = "SELECT '3333-03-03'::datee > '4444-04-04'::datee;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '3333-03-03'::datee > '3333-03-03'::datee;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '3333-03-03'::datee > '2222-02-02'::datee;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

            def test_ge(self):
                sql = "SELECT '3333-03-03'::datee >= '4444-04-04'::datee;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '3333-03-03'::datee >= '3333-03-03'::datee;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3333-03-03'::datee >= '2222-02-02'::datee;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

            def test_lt(self):
                sql = "SELECT '3333-03-03'::datee < '4444-04-04'::datee;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3333-03-03'::datee < '3333-03-03'::datee;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '3333-03-03'::datee < '2222-02-02'::datee;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

            def test_le(self):
                sql = "SELECT '3333-03-03'::datee <= '4444-04-04'::datee;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3333-03-03'::datee <= '3333-03-03'::datee;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3333-03-03'::datee <= '2222-02-02'::datee;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

            def test_eq(self):
                sql = "SELECT '3333-03-03'::datee = '4444-04-04'::datee;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '3333-03-03'::datee = '3333-03-03'::datee;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3333-03-03'::datee = '2222-02-02'::datee;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

            def test_ne(self):
                sql = "SELECT '3333-03-03'::datee != '4444-04-04'::datee;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3333-03-03'::datee != '3333-03-03'::datee;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '3333-03-03'::datee != '2222-02-02'::datee;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

        class TestIdxAndAggr:

            def test_init(self):
                sql = '''
                    DROP TABLE IF EXISTS r;
                    CREATE TABLE r (d datee);
                    INSERT INTO r VALUES (NULL), ('null:300'), (NULL), ('5555-05-05'), ('null:500'),
                                         ('2222-02-02'), ('null:200'), ('null:400'), (NULL), (NULL),
                                         ('3333-03-03'), ('1111-01-01'), ('null:100'), ('4444-04-04'), (NULL);
                '''
                flag, result = query(sql, 0)
                assert flag == 1

            def test_btree_index(self):
                sql = "CREATE INDEX r_index ON r USING btree(d mn_datee_ops);"
                flag, result = query(sql, 0)
                assert flag == 1
                sql = "SELECT t.* FROM r t;"
                flag, result = query(sql, 2)
                assert flag == 1
                assert len(result) == 15

            def test_order_by(self):
                sql = "SELECT * FROM r ORDER BY d;"
                flag, result = query(sql, 2)
                assert flag == 1
                assert result == [('NULL:100',), ('NULL:200',), ('NULL:300',), ('NULL:400',), ('NULL:500',),
                                  ('1111-01-01',), ('2222-02-02',), ('3333-03-03',), ('4444-04-04',), ('5555-05-05',),
                                  (None,), (None,), (None,), (None,), (None,)]

            def test_is_const(self):
                sql = "SELECT * FROM r WHERE is_const(d);"
                flag, result = query(sql, 2)
                assert flag == 1
                assert len(result) == 5

            def test_is_marked_null(self):
                sql = "SELECT * FROM r WHERE is_marked_null(d);"
                flag, result = query(sql, 2)
                assert flag == 1
                assert len(result) == 5

            def test_max(self):
                sql = "SELECT max(d) FROM r;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert str(result) == '5555-05-05'

            def test_min(self):
                sql = "SELECT min(d) FROM r;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert str(result) == '1111-01-01'

        class TestHash:

            def test_init(self):
                sql = '''
                    DROP TABLE IF EXISTS rr;
                    CREATE TABLE rr (a datee, b intt);
                    INSERT INTO rr VALUES
                        ('0123-01-01', 100), ('0123-01-01', 200), ('0123-01-01', 300), ('0123-01-01', 400),
                        ('1234-05-05', 100), ('1234-05-05', 200), ('1234-05-05', 300),
                        ('2345-12-12', 100);
                '''
                flag, result = query(sql, 0)
                assert flag == 1

            def test_hash_results(self):
                sql = 'SELECT a, sum(b) FROM rr GROUP BY a ORDER BY a;'
                flag, result = query(sql, 2)
                assert flag == 1
                assert result == [('0123-01-01', 1000,), ('1234-05-05', 600,), ('2345-12-12', 100)]

            def test_hash_execution_plan(self):
                sql = 'EXPLAIN SELECT a, sum(b) FROM rr GROUP BY a;'
                flag, result = query(sql, 1)
                assert flag == 1
                assert 'HashAggregate' in result

    class TestNumericc:

        class TestIO:

            def test_constant(self):
                sql = "SELECT '0'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '123'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 123

                sql = "SELECT '-123'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == -123

                sql = "SELECT '123.456'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert float(result) == 123.456

                sql = "SELECT '999998888877777666665555544444333332222211111.0123456789'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert str(result) == '999998888877777666665555544444333332222211111.0123456789'

                sql = "SELECT 'nan'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert str(result) == 'NaN'

            def test_constant_illegal(self):
                sql = "SELECT ''::numericc;"
                flag, result = query(sql, 1)
                assert flag == -1

                sql = "SELECT 'abcde'::numericc;"
                flag, result = query(sql, 1)
                assert flag == -1

                sql = "SELECT '123.456.789'::numericc;"
                flag, result = query(sql, 1)
                assert flag == -1

                sql = "SELECT '  NaN aN'::numericc;"
                flag, result = query(sql, 1)
                assert flag == -1

            def test_markednull(self):
                sql = "SELECT 'nUlL:123'::intt;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result == 'NULL:123'

            def test_markednull_illegal(self):
                sql = "SELECT 'null:0'::numericc;"
                flag, result = query(sql, 1)
                assert flag == -1

                sql = "SELECT 'null:-1'::numericc;"
                flag, result = query(sql, 1)
                assert flag == -1

                sql = "SELECT 'null:abcde'::numericc;"
                flag, result = query(sql, 1)
                assert flag == -1

                sql = "SELECT 'null:abcde'::numericc;"
                flag, result = query(sql, 1)
                assert flag == -1

                sql = "SELECT 'null:'::numericc;"
                flag, result = query(sql, 1)
                assert flag == -1

            def test_sqlnull(self):
                sql = "SELECT NULL::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

        class TestCast:

            def test_from_base(self):
                sql = "SELECT '0'::numeric::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '123'::numeric::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 123

                sql = "SELECT '-123'::numeric::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == -123

                sql = "SELECT '123.456'::numeric::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert float(result) == 123.456

                sql = "SELECT '999998888877777666665555544444333332222211111.0123456789'::numeric::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert str(result) == '999998888877777666665555544444333332222211111.0123456789'

                sql = "SELECT 'nan'::numeric::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert str(result) == 'NaN'

            def test_to_base(self):
                sql = "SELECT '0'::numericc::numeric;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '123'::numericc::numeric;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 123

                sql = "SELECT '-123'::numericc::numeric;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == -123

                sql = "SELECT '123.456'::numericc::numeric;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert float(result) == 123.456

                sql = "SELECT '999998888877777666665555544444333332222211111.0123456789'::numericc::numeric;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert str(result) == '999998888877777666665555544444333332222211111.0123456789'

                sql = "SELECT 'nan'::numericc::numeric;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert str(result) == 'NaN'

                sql = "SELECT 'null:123'::numericc::numeric;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

        class TestMathOpt:

            def test_add(self):
                sql = "SELECT + '300'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 300

                sql = "SELECT '100'::numericc + '200'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 300

                sql = "SELECT 'null:100'::numericc + '200'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT '100'::numericc + 'null:200'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

            def test_minus(self):
                sql = "SELECT - '100'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == -100

                sql = "SELECT '300'::numericc - '200'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 100

                sql = "SELECT 'null:300'::numericc - '200'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT '300'::numericc - 'null:200'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

            def test_multiply(self):
                sql = "SELECT '100'::numericc * '200'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 20000

                sql = "SELECT 'null:100'::numericc * '200'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT '100'::numericc * 'null:200'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

            def test_divide(self):
                sql = "SELECT '200'::numericc / '100'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert float(result) == 2

                sql = "SELECT 'null:200'::numericc / '100'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT '200'::numericc / 'null:100'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

            def test_modulo(self):
                sql = "SELECT '108'::numericc % '10'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 8

                sql = "SELECT 'null:108'::numericc % '10'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT '108'::numericc % 'null:10'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

            def test_exponential(self):
                sql = "SELECT '2'::numericc ^ '3'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert float(result) == 8

                sql = "SELECT 'null:2'::numericc ^ '3'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT '2'::numericc ^ 'null:3'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

            def test_absolute(self):
                sql = "SELECT @'-8'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 8

                sql = "SELECT @'0'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT @'null:8'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

        class TestCprOpt:

            def test_gt(self):
                sql = "SELECT '3'::numericc > '4'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '3'::numericc > '3'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '3'::numericc > '2'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3'::numericc > 'null:4'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT 'null:3'::numericc > '3'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT 'null:3'::numericc > 'null:2'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3'::numericc |> '4'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '3'::numericc |> '3'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '3'::numericc |> '2'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3'::numericc |> 'null:4'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT 'null:3'::numericc |> '3'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT 'null:3'::numericc |> 'null:2'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None
                
                sql = "SELECT 'null:3'::numericc |> 'null:3'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

            # TODO: change test_ge,lt,le,eq,ne, add null:n op null:n
            def test_ge(self):
                sql = "SELECT '3'::numericc >= '4'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '3'::numericc >= '3'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3'::numericc >= '2'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3'::numericc >= 'null:4'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT 'null:3'::numericc >= '3'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT 'null:3'::numericc >= 'null:2'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3'::numericc |>= '4'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '3'::numericc |>= '3'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3'::numericc |>= '2'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3'::numericc |>= 'null:4'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT 'null:3'::numericc |>= '3'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT 'null:3'::numericc |>= 'null:2'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None
                
                sql = "SELECT 'null:3'::numericc |>= 'null:3'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

            def test_lt(self):
                sql = "SELECT '3'::numericc < '4'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3'::numericc < '3'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '3'::numericc < '2'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '3'::numericc < 'null:4'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT 'null:3'::numericc < '3'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT 'null:3'::numericc < 'null:2'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '3'::numericc |< '4'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3'::numericc |< '3'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '3'::numericc |< '2'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '3'::numericc |< 'null:4'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT 'null:3'::numericc |< '3'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT 'null:3'::numericc |< 'null:2'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None
                
                sql = "SELECT 'null:3'::numericc |< 'null:3'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

            def test_le(self):
                sql = "SELECT '3'::numericc <= '4'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3'::numericc <= '3'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3'::numericc <= '2'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '3'::numericc <= 'null:4'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT 'null:3'::numericc <= '3'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT 'null:3'::numericc <= 'null:2'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '3'::numericc |<= '4'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3'::numericc |<= '3'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3'::numericc |<= '2'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '3'::numericc |<= 'null:4'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT 'null:3'::numericc |<= '3'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT 'null:3'::numericc |<= 'null:2'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None
                
                sql = "SELECT 'null:3'::numericc |<= 'null:3'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

            def test_eq(self):
                sql = "SELECT '3'::numericc = '4'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '3'::numericc = '3'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3'::numericc = '2'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '3'::numericc = 'null:4'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT 'null:3'::numericc = '3'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT 'null:3'::numericc = 'null:2'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '3'::numericc |= '4'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '3'::numericc |= '3'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3'::numericc |= '2'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '3'::numericc |= 'null:4'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT 'null:3'::numericc |= '3'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT 'null:3'::numericc |= 'null:2'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None
                
                sql = "SELECT 'null:3'::numericc |= 'null:3'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

            def test_ne(self):
                sql = "SELECT '3'::numericc <> '4'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3'::numericc <> '3'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '3'::numericc <> '2'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3'::numericc <> 'null:4'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT 'null:3'::numericc <> '3'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT 'null:3'::numericc <> 'null:2'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3'::numericc |<> '4'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3'::numericc |<> '3'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

                sql = "SELECT '3'::numericc |<> '2'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 1

                sql = "SELECT '3'::numericc |<> 'null:4'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT 'null:3'::numericc |<> '3'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None

                sql = "SELECT 'null:3'::numericc |<> 'null:2'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert result is None
                
                sql = "SELECT 'null:3'::numericc |<> 'null:3'::numericc;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 0

        class TestIdxAndAggr:

            def test_init(self):
                sql = '''
                    DROP TABLE IF EXISTS r;
                    CREATE TABLE r (i numericc);
                    INSERT INTO r VALUES (NULL), ('null:300'), (NULL), ('555.555'), ('null:500'),
                                         ('222.222'), ('null:200'), ('null:400'), (NULL), (NULL),
                                         ('333.333'), ('111.111'), ('null:100'), ('444.444'), (NULL);
                '''
                flag, result = query(sql, 0)
                assert flag == 1

            def test_btree_index(self):
                sql = "CREATE INDEX r_index ON r USING btree(i mn_numericc_ops);"
                flag, result = query(sql, 0)
                assert flag == 1
                sql = "SELECT t.* FROM r t;"
                flag, result = query(sql, 2)
                assert flag == 1
                assert len(result) == 15

            def test_order_by(self):
                sql = "SELECT * FROM r ORDER BY i;"
                flag, result = query(sql, 2)
                assert flag == 1
                assert result == [('NULL:100',), ('NULL:200',), ('NULL:300',), ('NULL:400',), ('NULL:500',),
                                  ('111.111',), ('222.222',), ('333.333',), ('444.444',), ('555.555',),
                                  (None,), (None,), (None,), (None,), (None,)]

            def test_is_const(self):
                sql = "SELECT * FROM r WHERE is_const(i);"
                flag, result = query(sql, 2)
                assert flag == 1
                assert len(result) == 5

            def test_is_marked_null(self):
                sql = "SELECT * FROM r WHERE is_marked_null(i);"
                flag, result = query(sql, 2)
                assert flag == 1
                assert len(result) == 5

            def test_sum(self):
                sql = "SELECT sum(i) FROM r;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert float(result) == 1666.665

            def test_max(self):
                sql = "SELECT max(i) FROM r;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert float(result) == 555.555

            def test_min(self):
                sql = "SELECT min(i) FROM r;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert str(result) == '111.111'

            def test_avg(self):
                sql = "SELECT avg(i) FROM r;"
                flag, result = query(sql, 1)
                assert flag == 1
                assert float(result) == 333.333

        class TestHash:

            def test_init(self):
                sql = '''
                    DROP TABLE IF EXISTS rr;
                    CREATE TABLE rr (a numericc, b numericc);
                    INSERT INTO rr VALUES
                        (1, 11.1), (1, 22.2), (1, 33.3), (1, 44.4),
                        (2, 11.1), (2, 22.2), (2, 33.3),
                        (3, 11.1);
                '''
                flag, result = query(sql, 0)
                assert flag == 1

            def test_hash_results(self):
                sql = 'SELECT a, sum(b) FROM rr GROUP BY a ORDER BY a;'
                flag, results = query(sql, 2)
                assert flag == 1
                assert float(results[0][1]) == 111.0
                assert float(results[1][1]) == 66.6
                assert float(results[2][1]) == 11.1

            def test_hash_execution_plan(self):
                sql = 'EXPLAIN SELECT a, sum(b) FROM rr GROUP BY a;'
                flag, result = query(sql, 1)
                assert flag == 1
                assert 'HashAggregate' in result

        class TestPartialAggr:

            def test_init(self):
                sql = '''
                    DROP TABLE IF EXISTS rrr;
                    CREATE TABLE rrr (a numericc, b numericc);
                '''
                query(sql, 0)
                sql = 'INSERT INTO rrr VALUES '
                sql_part = '(1, 1.1), (2, 2.2), (3, \'null:3\'), (4, NULL), '
                for i in range(150000):
                    sql += sql_part
                sql += '(0, -500), (5, 500);'
                flag, result = query(sql, 0)
                assert flag == 1

            def test_sum(self):
                sql = 'SELECT sum(b) from rrr;'
                flag, result = query(sql, 1)
                assert flag == 1
                assert float(result) == 495000

            def test_sum_group_by(self):
                sql = 'SELECT a, sum(rrr.b) from rrr group by a order by a;'
                flag, results = query(sql, 2)
                assert flag == 1
                assert float(results[0][1]) == -500
                assert float(results[1][1]) == 165000
                assert float(results[2][1]) == 330000
                assert results[3][1] is None
                assert results[4][1] is None
                assert float(results[5][1]) == 500

            def test_max(self):
                sql = 'SELECT max(b) from rrr;'
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == 500

            def test_min(self):
                sql = 'SELECT min(b) from rrr;'
                flag, result = query(sql, 1)
                assert flag == 1
                assert int(result) == -500

            def test_avg(self):
                sql = 'SELECT avg(b) from rrr;'
                flag, result = query(sql, 1)
                assert flag == 1
                assert float(result) - 1.65 < 0.00001

            def test_avg_group_by(self):
                sql = 'SELECT a, avg(rrr.b) from rrr group by a order by a;'
                flag, results = query(sql, 2)
                assert flag == 1
                assert float(results[0][1]) == -500
                assert float(results[1][1]) == 1.1
                assert float(results[2][1]) == 2.2
                assert results[3][1] is None
                assert results[4][1] is None
                assert float(results[5][1]) == 500
