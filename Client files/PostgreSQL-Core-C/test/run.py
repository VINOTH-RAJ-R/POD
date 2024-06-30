#! /usr/bin/python

import pytest
import psycopg
import configparser
import random
import sys
import numpy as np
import datetime
import ipsum

repitition = 1000
db_config = configparser.ConfigParser()
db_config.read("test.config")
pgsql = db_config["pgsql"]


def query(conn, sql, fetch=1):
    cur = conn.cursor()
    to_return = [True, None]
    try:
        cur.execute(sql)
        if fetch == 1:
            to_return[1] = cur.fetchone()[0]
        elif fetch == 2:
            to_return[1] = cur.fetchall()
    except Exception as e:
        to_return[0] = False
        to_return[1] = e
        conn.rollback()
    return to_return


UINT_MAX = 4294967295
SINT_MIN = -32768
SINT_MAX = 32767
INT_MIN  = -2147483648
INT_MAX  = 2147483647
BINT_MIN = -9223372036854775808
BINT_MAX = 9223372036854775807
NUM_MIN  = sys.float_info.min
NUM_MAX  = sys.float_info.max
DATE_MAX = 2147483493


class MyRandom:

    model = ipsum.load_model('en')

    def rNullID():
        return random.randint(0, UINT_MAX)

    def rSmallint():
        return random.randint(SINT_MIN, SINT_MAX)

    def rInt():
        return random.randint(INT_MIN, INT_MAX)
    
    def rBigint():
        return random.randint(BINT_MIN, BINT_MAX)
    
    def rNumeric():
        return random.uniform(NUM_MIN, NUM_MAX)

    def rReal():
        return np.single(MyRandom.rNumeric())
    
    def rDouble():
        return np.double(MyRandom.rNumeric())

    def rDate():
        return 'J' + str(random.randint(0, DATE_MAX))

    def rDateDDMMYY():
        year = random.randint(1000, 3000)
        month = random.randint(1, 12)
        if month in [1, 3, 5, 7, 8, 10, 12]:
            date = random.randint(1, 31)
        elif month in [4, 6, 9, 11]:
            date = random.randint(1, 30)
        else:
            if (year % 400 == 0) or (year % 4 == 0 and year % 100 != 0):
                date = random.randint(1, 29)
            else:
                date = random.randint(1, 28)
        
        if month < 10:
            month = '0' + str(month)
        if date < 10:
            date = '0' + str(date)

        return "{}-{}-{}".format(year, month, date)
    
    def rText():
        return '\n'.join(MyRandom.model.generate_paragraphs(10)).replace('\'', '\'\'')
    
    def rWord():
        return MyRandom.model.generate_words(1)[0]


@pytest.fixture(scope='function')
def setup_clean():
    conn = psycopg.connect(
        dbname=pgsql["dbname"],
        user=pgsql['user'],
        password=pgsql['password'],
        host=pgsql['host'],
        port=pgsql['port']
    )
    yield conn
    conn.commit()
    conn.close()


class TestBasic:

    def test_connection_clean(self, setup_clean):
        conn = setup_clean
        sql = 'SELECT 1 + 1;'
        success, result = query(conn, sql)
        assert success == True
        assert result == 2
    
    def test_extension(self, setup_clean):
        conn = setup_clean
        sql = 'SELECT mn_version();'
        success, result = query(conn, sql)
        assert success == True


class TestIntt:

    class TestIO:

        def test_marked_constant(self, setup_clean):
            conn = setup_clean
            for _ in range (repitition):
                number = MyRandom.rInt()
                sql = "SELECT '{}'::INTT;".format(number)
                success, result = query(conn, sql)
                assert success == True
                assert int(result) == number
        
        def test_marked_null(self, setup_clean):
            conn = setup_clean
            for _ in range (repitition):
                id = "NULL:" + str(MyRandom.rNullID())
                sql = "SELECT '{}'::INTT;".format(id)
                success, result = query(conn, sql)
                assert success == True
                assert result == id
        
        def test_sql_null(self, setup_clean):
            conn = setup_clean
            sql = "SELECT NULL::INTT;"
            success, result = query(conn, sql)
            assert success == True
            assert result is None

        def test_illegal1(self, setup_clean):
            conn = setup_clean
            id = "NULL:" + str(MyRandom.rDate())
            sql = "SELECT '{}'::INTT;".format(id)
            success, result = query(conn, sql)
            assert success == False
        
        def test_illegal2(self, setup_clean):
            conn = setup_clean
            id = "NULL:" + str(MyRandom.rDouble())
            sql = "SELECT '{}'::INTT;".format(id)
            success, result = query(conn, sql)
            assert success == False
        
        def test_illegal3(self, setup_clean):
            conn = setup_clean
            id = "NULL:" + str(MyRandom.rText())
            sql = "SELECT '{}'::INTT;".format(id)
            success, result = query(conn, sql)
            assert success == False
        
        def test_illegal4(self, setup_clean):
            conn = setup_clean
            id = "NULL:" + str(MyRandom.rWord())
            sql = "SELECT '{}'::INTT;".format(id)
            success, result = query(conn, sql)
            assert success == False
        
        def test_illegal5(self, setup_clean):
            conn = setup_clean
            id = "NULL:" + str(0 - MyRandom.rNullID())
            sql = "SELECT '{}'::INTT;".format(id)
            success, result = query(conn, sql)
            assert success == False
        
        def test_illegal6(self, setup_clean):
            conn = setup_clean
            sql = "SELECT 'null:'::INTT;"
            success, result = query(conn, sql)
            assert success == False


    class TestCast:
        
        def test_base_to_marked(self, setup_clean):
            conn = setup_clean
            for _ in range (repitition):
                number = MyRandom.rInt()
                sql = "SELECT {}::INT::INTT;".format(number)
                success, result = query(conn, sql)
                assert success == True
                assert int(result) == number

        def test_marked_to_base(self, setup_clean):
            conn = setup_clean
            for _ in range (repitition):
                number = MyRandom.rInt()
                sql = "SELECT '{}'::INTT::INT;".format(number)
                success, result = query(conn, sql)
                assert success == True
                assert int(result) == number

        def test_marked_null_to_base(self, setup_clean):
            conn = setup_clean
            for _ in range (repitition):
                id = MyRandom.rNullID()
                sql = "SELECT 'NULL:{}'::INTT::INT;".format(id)
                success, result = query(conn, sql)
                assert success == True
                assert result == None
        
        def test_sql_null_base_to_marked(self, setup_clean):
            conn = setup_clean
            id = MyRandom.rNullID()
            sql = "SELECT NULL::INT::INTT;".format(id)
            success, result = query(conn, sql)
            assert success == True
            assert result == None
        
        def test_sql_null_marked_to_base(self, setup_clean):
            conn = setup_clean
            id = MyRandom.rNullID()
            sql = "SELECT NULL::INTT::INT;".format(id)
            success, result = query(conn, sql)
            assert success == True
            assert result == None


    class TestComparison:


        class TestMarkedCompareMarked:

            comparison_sql = """
            SELECT
                '{0}'::INTT <  '{1}'::INTT,
                '{0}'::INTT <= '{1}'::INTT,
                '{0}'::INTT =  '{1}'::INTT,
                '{0}'::INTT >= '{1}'::INTT,
                '{0}'::INTT >  '{1}'::INTT,
                '{0}'::INTT <> '{1}'::INTT;
            """

            def test_constant_constant(self, setup_clean):
                conn = setup_clean
                for _ in range (repitition):
                    left  = MyRandom.rInt()
                    right = MyRandom.rInt()
                    sql = self.comparison_sql.format(left, right)
                    success, result = query(conn, sql, fetch=2)
                    assert success == True
                    assert result == [(
                        left <  right,
                        left <= right,
                        left == right,
                        left >= right,
                        left >  right,
                        left != right
                    )]
            
            def test_constant_markednull(self, setup_clean):
                conn = setup_clean
                for _ in range (repitition):
                    left  = MyRandom.rInt()
                    right = MyRandom.rNullID()
                    sql = self.comparison_sql.format(left, "NULL:" + str(right))
                    success, result = query(conn, sql, fetch=2)
                    assert success == True
                    assert result == [(
                        False, False, False, True, True, True
                    )]

            def test_markednull_constant(self, setup_clean):
                conn = setup_clean
                for _ in range (repitition):
                    left  = MyRandom.rNullID()
                    right = MyRandom.rInt()
                    sql = self.comparison_sql.format("NULL:" + str(left), right)
                    success, result = query(conn, sql, fetch=2)
                    assert success == True
                    assert result == [(
                        True, True, False, False, False, True
                    )]

            def test_markednull_markednull(self, setup_clean):
                conn = setup_clean
                for _ in range (repitition):
                    left  = MyRandom.rNullID()
                    right = MyRandom.rNullID()
                    sql = self.comparison_sql.format("NULL:" + str(left), "NULL:" + str(right))
                    success, result = query(conn, sql, fetch=2)
                    assert success == True
                    assert result == [(
                        left <  right,
                        left <= right,
                        left == right,
                        left >= right,
                        left >  right,
                        left != right
                    )]


        class TestMarkedCompareMarkedWithNull:

            comparison_sql = """
            SELECT
                {0}::INTT <  {1}::INTT,
                {0}::INTT <= {1}::INTT,
                {0}::INTT =  {1}::INTT,
                {0}::INTT >= {1}::INTT,
                {0}::INTT >  {1}::INTT,
                {0}::INTT <> {1}::INTT;
            """

            def test_constant_sqlnull(self, setup_clean):
                conn = setup_clean
                for _ in range (repitition):
                    left  = MyRandom.rInt()
                    sql = self.comparison_sql.format("'{}'".format(left), "NULL")
                    success, result = query(conn, sql, fetch=2)
                    assert success == True
                    assert result == [(
                        None, None, None, None, None, None
                    )]
            
            def test_markednull_sqlnull(self, setup_clean):
                conn = setup_clean
                for _ in range (repitition):
                    left  = MyRandom.rNullID()
                    sql = self.comparison_sql.format("'NULL:{}'".format(left), "NULL")
                    success, result = query(conn, sql, fetch=2)
                    assert success == True
                    assert result == [(
                        None, None, None, None, None, None
                    )]

            def test_sqlnull_sqlnull(self, setup_clean):
                conn = setup_clean
                for _ in range (repitition):
                    sql = self.comparison_sql.format("NULL", "NULL")
                    success, result = query(conn, sql, fetch=2)
                    assert success == True
                    assert result == [(
                        None, None, None, None, None, None
                    )]

            def test_sqlnull_constant(self, setup_clean):
                conn = setup_clean
                for _ in range (repitition):
                    right = MyRandom.rInt()
                    sql = self.comparison_sql.format("NULL", "'{}'".format(right))
                    success, result = query(conn, sql, fetch=2)
                    assert success == True
                    assert result == [(
                        None, None, None, None, None, None
                    )]

            def test_sqlnull_markednull(self, setup_clean):
                conn = setup_clean
                for _ in range (repitition):
                    right = MyRandom.rNullID()
                    sql = self.comparison_sql.format("NULL", "'NULL:{}'".format(right))
                    success, result = query(conn, sql, fetch=2)
                    assert success == True
                    assert result == [(
                        None, None, None, None, None, None
                    )]
        

        class TestMarkedCompareBase:

            comparison_sql = """
            SELECT
                '{0}'::INTT <  '{1}'::INT,
                '{0}'::INTT <= '{1}'::INT,
                '{0}'::INTT =  '{1}'::INT,
                '{0}'::INTT >= '{1}'::INT,
                '{0}'::INTT >  '{1}'::INT,
                '{0}'::INTT <> '{1}'::INT;
            """

            def test_constant_constant(self, setup_clean):
                conn = setup_clean
                for _ in range (repitition):
                    left  = MyRandom.rInt()
                    right = MyRandom.rInt()
                    sql = self.comparison_sql.format(left, right)
                    success, result = query(conn, sql, fetch=2)
                    assert success == True
                    assert result == [(
                        left <  right,
                        left <= right,
                        left == right,
                        left >= right,
                        left >  right,
                        left != right
                    )]

            def test_markednull_constant(self, setup_clean):
                conn = setup_clean
                for _ in range (repitition):
                    left  = MyRandom.rNullID()
                    right = MyRandom.rInt()
                    sql = self.comparison_sql.format("NULL:" + str(left), right)
                    success, result = query(conn, sql, fetch=2)
                    assert success == True
                    assert result == [(
                        True, True, False, False, False, True
                    )]

        
        class TestMarkedCompareBaseWithNull:

            comparison_sql = """
            SELECT
                {0}::INTT <  {1}::INT,
                {0}::INTT <= {1}::INT,
                {0}::INTT =  {1}::INT,
                {0}::INTT >= {1}::INT,
                {0}::INTT >  {1}::INT,
                {0}::INTT <> {1}::INT;
            """

            def test_constant_sqlnull(self, setup_clean):
                conn = setup_clean
                for _ in range (repitition):
                    left  = MyRandom.rInt()
                    sql = self.comparison_sql.format("'{}'".format(left), "NULL")
                    success, result = query(conn, sql, fetch=2)
                    assert success == True
                    assert result == [(
                        None, None, None, None, None, None
                    )]

            def test_markednull_sqlnull(self, setup_clean):
                conn = setup_clean
                for _ in range (repitition):
                    left  = MyRandom.rNullID()
                    sql = self.comparison_sql.format("'NULL:{}'".format(left), "NULL")
                    success, result = query(conn, sql, fetch=2)
                    assert success == True
                    assert result == [(
                        None, None, None, None, None, None
                    )]

            def test_sqlnull_sqlnull(self, setup_clean):
                conn = setup_clean
                for _ in range (repitition):
                    sql = self.comparison_sql.format("NULL", "NULL")
                    success, result = query(conn, sql, fetch=2)
                    assert success == True
                    assert result == [(
                        None, None, None, None, None, None
                    )]

            def test_sqlnull_constant(self, setup_clean):
                conn = setup_clean
                for _ in range (repitition):
                    right = MyRandom.rInt()
                    sql = self.comparison_sql.format("NULL", "'{}'".format(right))
                    success, result = query(conn, sql, fetch=2)
                    assert success == True
                    assert result == [(
                        None, None, None, None, None, None
                    )]


        class TestBaseCompareMarked:

            comparison_sql = """
            SELECT
                '{0}'::INT <  '{1}'::INTT,
                '{0}'::INT <= '{1}'::INTT,
                '{0}'::INT =  '{1}'::INTT,
                '{0}'::INT >= '{1}'::INTT,
                '{0}'::INT >  '{1}'::INTT,
                '{0}'::INT <> '{1}'::INTT;
            """

            def test_constant_constant(self, setup_clean):
                conn = setup_clean
                for _ in range (repitition):
                    left  = MyRandom.rInt()
                    right = MyRandom.rInt()
                    sql = self.comparison_sql.format(left, right)
                    success, result = query(conn, sql, fetch=2)
                    assert success == True
                    assert result == [(
                        left <  right,
                        left <= right,
                        left == right,
                        left >= right,
                        left >  right,
                        left != right
                    )]

            def test_constant_markednull(self, setup_clean):
                conn = setup_clean
                for _ in range (repitition):
                    left  = MyRandom.rInt()
                    right = MyRandom.rNullID()
                    sql = self.comparison_sql.format(left, "NULL:" + str(right))
                    success, result = query(conn, sql, fetch=2)
                    assert success == True
                    assert result == [(
                        False, False, False, True, True, True
                    )]

        
        class TestBaseCompareMarkedWithNull:

            comparison_sql = """
            SELECT
                {0}::INT <  {1}::INTT,
                {0}::INT <= {1}::INTT,
                {0}::INT =  {1}::INTT,
                {0}::INT >= {1}::INTT,
                {0}::INT >  {1}::INTT,
                {0}::INT <> {1}::INTT;
            """

            def test_constant_sqlnull(self, setup_clean):
                conn = setup_clean
                for _ in range (repitition):
                    left  = MyRandom.rInt()
                    sql = self.comparison_sql.format("'{}'".format(left), "NULL")
                    success, result = query(conn, sql, fetch=2)
                    assert success == True
                    assert result == [(
                        None, None, None, None, None, None
                    )]

            def test_sqlnull_sqlnull(self, setup_clean):
                conn = setup_clean
                for _ in range (repitition):
                    sql = self.comparison_sql.format("NULL", "NULL")
                    success, result = query(conn, sql, fetch=2)
                    assert success == True
                    assert result == [(
                        None, None, None, None, None, None
                    )]

            def test_sqlnull_constant(self, setup_clean):
                conn = setup_clean
                for _ in range (repitition):
                    right = MyRandom.rInt()
                    sql = self.comparison_sql.format("NULL", "'{}'".format(right))
                    success, result = query(conn, sql, fetch=2)
                    assert success == True
                    assert result == [(
                        None, None, None, None, None, None
                    )]

            def test_sqlnull_markednull(self, setup_clean):
                conn = setup_clean
                for _ in range (repitition):
                    right = MyRandom.rNullID()
                    sql = self.comparison_sql.format("NULL", "'NULL:{}'".format(right))
                    success, result = query(conn, sql, fetch=2)
                    assert success == True
                    assert result == [(
                        None, None, None, None, None, None
                    )]


    class TestOperatorExceptDivAndMod:


        class TestMarkedOperatorMarked:

            operator_sql = """
            SELECT
                '{0}'::INTT + '{1}'::INTT,
                '{0}'::INTT - '{1}'::INTT,
                '{0}'::INTT * '{1}'::INTT,
                + '{1}'::INTT,
                - '{1}'::INTT;
            """

            def test_constant_constant(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):

                    left  = MyRandom.rInt()
                    right = MyRandom.rInt()
                    sql = self.operator_sql.format(left, right)
                    success, result = query(conn, sql, fetch=2)

                    add = left + right
                    sub = left - right
                    mul = left * right
                    uad = right
                    neg = 0 - right

                    if  not (INT_MIN < add < INT_MAX) or \
                        not (INT_MIN < sub < INT_MAX) or \
                        not (INT_MIN < mul < INT_MAX) or \
                        not (INT_MIN < neg < INT_MAX):
                        assert success == False
                        continue

                    if success == False:
                        print("""
                        \n
                        LEFT: {}, RIGHT: {},
                        ERROR: {},
                        """.format(left, right, result))

                    assert success == True
                    assert [tuple([int(x) for x in result[0]])] == [(
                        add, sub, mul, uad, neg
                    )]

            def test_constant_markednull(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):
                    left  = MyRandom.rInt()
                    right = MyRandom.rNullID()
                    sql = self.operator_sql.format(left, "NULL:%d"%(right))
                    success, result = query(conn, sql, fetch=2)
                    assert success == True
                    if left == 0 or left == 1:
                        continue
                    else:
                        assert result == [(
                            None, None, None, "NULL:%d"%(right), None
                        )]
            
            def test_constant_markednull_special_0(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):
                    left  = 0
                    right = MyRandom.rNullID()
                    sql = self.operator_sql.format(left, "NULL:%d"%(right))
                    success, result = query(conn, sql, fetch=2)
                    assert success == True
                    assert result == [(
                        "NULL:%d"%(right), None, "0", "NULL:%d"%(right), None
                    )]

            def test_constant_markednull_special_1(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):
                    left  = 1
                    right = MyRandom.rNullID()
                    sql = self.operator_sql.format(left, "NULL:%d"%(right))
                    success, result = query(conn, sql, fetch=2)
                    assert success == True
                    assert result == [(
                        None, None, "NULL:%d"%(right), "NULL:%d"%(right), None
                    )]

            def test_markednull_constant(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):
                    left  = MyRandom.rNullID()
                    right = MyRandom.rInt()
                    sql = self.operator_sql.format("NULL:%d"%(left), right)
                    success, result = query(conn, sql, fetch=2)
                    assert success == True
                    if right == 0 or right == 1:
                        continue
                    else:
                        assert result == [(
                            None, None, None, str(right), str(0 - right)
                        )]

            def test_markednull_constant_special_0(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):
                    left  = MyRandom.rNullID()
                    right = 0
                    sql = self.operator_sql.format("NULL:%d"%(left), right)
                    success, result = query(conn, sql, fetch=2)
                    assert success == True
                    assert result == [(
                        "NULL:%d"%(left), "NULL:%d"%(left), "0", "0", "0"
                    )]

            def test_markednull_constant_special_1(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):
                    left  = MyRandom.rNullID()
                    right = 1
                    sql = self.operator_sql.format("NULL:%d"%(left), right)
                    success, result = query(conn, sql, fetch=2)
                    assert success == True
                    assert result == [(
                        None, None, "NULL:%d"%(left), "1", "-1"
                    )]

            def test_markednull_markednull(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):
                    left  = MyRandom.rNullID()
                    right = MyRandom.rNullID()
                    sql = self.operator_sql.format("NULL:%d"%(left), "NULL:%d"%(right))
                    success, result = query(conn, sql, fetch=2)
                    if left == right: continue
                    assert success == True
                    assert result == [(
                        None, None, None, "NULL:%d"%(right), None
                    )]
            
            def test_markednull_markednull_special_same(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):
                    left  = MyRandom.rNullID()
                    right = left
                    sql = self.operator_sql.format("NULL:%d"%(left), "NULL:%d"%(right))
                    success, result = query(conn, sql, fetch=2)
                    assert success == True
                    assert result == [(
                        None, "0", None, "NULL:%d"%(right), None
                    )]

        
        class TestMarkedOperatorMarkedWithNull:

            operator_sql = """
            SELECT
                {0}::INTT + {1}::INTT,
                {0}::INTT - {1}::INTT,
                {0}::INTT * {1}::INTT,
                + {1}::INTT,
                - {1}::INTT;
            """

            def test_constant_sqlnull(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):

                    left  = str(MyRandom.rInt())
                    right = "NULL"
                    sql = self.operator_sql.format(left, right)
                    success, result = query(conn, sql, fetch=2)

                    assert success == True
                    assert result == [(None, None, None, None, None)]

            def test_markednull_sqlnull(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):

                    left  = "'NULL:{}'".format(MyRandom.rNullID())
                    right = "NULL"
                    sql = self.operator_sql.format(left, right)
                    success, result = query(conn, sql, fetch=2)

                    assert success == True
                    assert result == [(None, None, None, None, None)]

            def test_sqlnull_sqlnull(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):

                    left  = "NULL"
                    right = "NULL"
                    sql = self.operator_sql.format(left, right)
                    success, result = query(conn, sql, fetch=2)

                    assert success == True
                    assert result == [(None, None, None, None, None)]

            def test_sqlnull_constant(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):

                    left  = "NULL"
                    right = MyRandom.rInt()
                    sql = self.operator_sql.format(left, str(right))
                    success, result = query(conn, sql, fetch=2)

                    assert success == True
                    assert result == [(None, None, None, str(right), str(0 - right))]

            def test_sqlnull_markednull(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):

                    left  = "NULL"
                    right = MyRandom.rNullID()
                    sql = self.operator_sql.format(left, "'NULL:{}'".format(right))
                    success, result = query(conn, sql, fetch=2)

                    assert success == True
                    assert result == [(None, None, None, "NULL:{}".format(right), None)]


        class TestMarkedOperatorBase:
            
            operator_sql = """
            SELECT
                '{0}'::INTT + '{1}'::INT,
                '{0}'::INTT - '{1}'::INT,
                '{0}'::INTT * '{1}'::INT,
                + '{1}'::INT,
                - '{1}'::INT;
            """
            
            def test_constant_constant(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):
                    
                    left  = MyRandom.rInt()
                    right = MyRandom.rInt()
                    sql = self.operator_sql.format(left, right)
                    success, result = query(conn, sql, fetch=2)

                    add = left + right
                    sub = left - right
                    mul = left * right
                    uad = right
                    neg = 0 - right

                    if  not (INT_MIN < add < INT_MAX) or \
                        not (INT_MIN < sub < INT_MAX) or \
                        not (INT_MIN < mul < INT_MAX) or \
                        not (INT_MIN < neg < INT_MAX):
                        assert success == False
                        continue

                    if success == False:
                        print("""
                        \n
                        LEFT: {}, RIGHT: {},
                        ERROR: {},
                        """.format(left, right, result))

                    assert success == True
                    assert [tuple([int(x) for x in result[0]])] == [(
                        add, sub, mul, uad, neg
                    )]

            def test_markednull_constant(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):
                    left  = MyRandom.rNullID()
                    right = MyRandom.rInt()
                    sql = self.operator_sql.format("NULL:%d"%(left), right)
                    success, result = query(conn, sql, fetch=2)
                    assert success == True
                    if right == 0 or right == 1:
                        continue
                    else:
                        assert result == [(
                            None, None, None, right, 0 - right
                        )]

            def test_markednull_constant_special_0(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):
                    left  = MyRandom.rNullID()
                    right = 0
                    sql = self.operator_sql.format("NULL:%d"%(left), right)
                    success, result = query(conn, sql, fetch=2)
                    assert success == True
                    assert result == [(
                        "NULL:%d"%(left), "NULL:%d"%(left), "0", 0, 0
                    )]

            def test_markednull_constant_special_1(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):
                    left  = MyRandom.rNullID()
                    right = 1
                    sql = self.operator_sql.format("NULL:%d"%(left), right)
                    success, result = query(conn, sql, fetch=2)
                    assert success == True
                    assert result == [(
                        None, None, "NULL:%d"%(left), 1, -1
                    )]


        class TestMarkedOperatorBaseWithNull:

            operator_sql = """
            SELECT
                {0}::INTT + {1}::INT,
                {0}::INTT - {1}::INT,
                {0}::INTT * {1}::INT,
                + {1}::INT,
                - {1}::INT;
            """

            def test_constant_sqlnull(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):

                    left  = str(MyRandom.rInt())
                    right = "NULL"
                    sql = self.operator_sql.format(left, right)
                    success, result = query(conn, sql, fetch=2)

                    assert success == True
                    assert result == [(None, None, None, None, None)]

            def test_markednull_sqlnull(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):

                    left  = "'NULL:{}'".format(MyRandom.rNullID())
                    right = "NULL"
                    sql = self.operator_sql.format(left, right)
                    success, result = query(conn, sql, fetch=2)

                    assert success == True
                    assert result == [(None, None, None, None, None)]

            def test_sqlnull_sqlnull(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):

                    left  = "NULL"
                    right = "NULL"
                    sql = self.operator_sql.format(left, right)
                    success, result = query(conn, sql, fetch=2)

                    assert success == True
                    assert result == [(None, None, None, None, None)]

            def test_sqlnull_constant(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):

                    left  = "NULL"
                    right = MyRandom.rInt()
                    sql = self.operator_sql.format(left, str(right))
                    success, result = query(conn, sql, fetch=2)

                    assert success == True
                    assert result == [(None, None, None, right, 0 - right)]


        class TestBaseOperatorMarked:

            operator_sql = """
            SELECT
                '{0}'::INT + '{1}'::INTT,
                '{0}'::INT - '{1}'::INTT,
                '{0}'::INT * '{1}'::INTT,
                + '{1}'::INTT,
                - '{1}'::INTT;
            """

            def test_constant_constant(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):

                    left  = MyRandom.rInt()
                    right = MyRandom.rInt()
                    sql = self.operator_sql.format(left, right)
                    success, result = query(conn, sql, fetch=2)

                    add = left + right
                    sub = left - right
                    mul = left * right
                    uad = right
                    neg = 0 - right

                    if  not (INT_MIN < add < INT_MAX) or \
                        not (INT_MIN < sub < INT_MAX) or \
                        not (INT_MIN < mul < INT_MAX) or \
                        not (INT_MIN < neg < INT_MAX):
                        assert success == False
                        continue

                    if success == False:
                        print("""
                        \n
                        LEFT: {}, RIGHT: {},
                        ERROR: {},
                        """.format(left, right, result))

                    assert success == True
                    assert [tuple([int(x) for x in result[0]])] == [(
                        add, sub, mul, uad, neg
                    )]
            
            def test_constant_markednull(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):
                    left  = MyRandom.rInt()
                    right = MyRandom.rNullID()
                    sql = self.operator_sql.format(left, "NULL:%d"%(right))
                    success, result = query(conn, sql, fetch=2)
                    assert success == True
                    if left == 0 or left == 1:
                        continue
                    else:
                        assert result == [(
                            None, None, None, "NULL:%d"%(right), None
                        )]
            
            def test_constant_markednull_special_0(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):
                    left  = 0
                    right = MyRandom.rNullID()
                    sql = self.operator_sql.format(left, "NULL:%d"%(right))
                    success, result = query(conn, sql, fetch=2)
                    assert success == True
                    assert result == [(
                        "NULL:%d"%(right), None, "0", "NULL:%d"%(right), None
                    )]
                
            def test_constant_markednull_special_1(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):
                    left  = 1
                    right = MyRandom.rNullID()
                    sql = self.operator_sql.format(left, "NULL:%d"%(right))
                    success, result = query(conn, sql, fetch=2)
                    assert success == True
                    assert result == [(
                        None, None, "NULL:%d"%(right), "NULL:%d"%(right), None
                    )]


        class TestBaseOperatorMarkedWithNull:

            operator_sql = """
            SELECT
                {0}::INT + {1}::INTT,
                {0}::INT - {1}::INTT,
                {0}::INT * {1}::INTT,
                + {1}::INTT,
                - {1}::INTT;
            """

            def test_constant_sqlnull(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):

                    left  = str(MyRandom.rInt())
                    right = "NULL"
                    sql = self.operator_sql.format(left, right)
                    success, result = query(conn, sql, fetch=2)

                    assert success == True
                    assert result == [(None, None, None, None, None)]
            
            def test_sqlnull_sqlnull(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):

                    left  = "NULL"
                    right = "NULL"
                    sql = self.operator_sql.format(left, right)
                    success, result = query(conn, sql, fetch=2)

                    assert success == True
                    assert result == [(None, None, None, None, None)]

            def test_sqlnull_constant(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):

                    left  = "NULL"
                    right = MyRandom.rInt()
                    sql = self.operator_sql.format(left, str(right))
                    success, result = query(conn, sql, fetch=2)

                    assert success == True
                    assert result == [(None, None, None, str(right), str(0 - right))]

            def test_sqlnull_markednull(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):

                    left  = "NULL"
                    right = MyRandom.rNullID()
                    sql = self.operator_sql.format(left, "'NULL:{}'".format(right))
                    success, result = query(conn, sql, fetch=2)

                    assert success == True
                    assert result == [(None, None, None, "NULL:{}".format(right), None)]


    class TestOperatorDiv:


        class TestMarkedOperatorMarked:

            operator_sql = """
            SELECT
                '{0}'::INTT / '{1}'::INTT;
            """

            def test_constant_constant(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):

                    left  = MyRandom.rInt()
                    right = MyRandom.rInt()
                    sql = self.operator_sql.format(left, right)
                    success, result = query(conn, sql)

                    if  right == 0:
                        assert success == False
                        continue

                    if success == False:
                        print("""
                        \n
                        LEFT: {}, RIGHT: {},
                        ERROR: {},
                        """.format(left, right, result))

                    assert success == True
                    assert result == str(int(left / right))

            def test_constant_markednull(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):
                    left  = MyRandom.rInt()
                    right = MyRandom.rNullID()
                    sql = self.operator_sql.format(left, "NULL:%d"%(right))
                    success, result = query(conn, sql)

                    assert success == True
                    if left == 0:
                        continue
                    else:
                        assert result == None

            def test_constant_markednull_special_0(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):
                    left  = 0
                    right = MyRandom.rNullID()
                    sql = self.operator_sql.format(left, "NULL:%d"%(right))
                    success, result = query(conn, sql)

                    assert success == True
                    assert result == '0'

            def test_markednull_constant(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):
                    left  = MyRandom.rNullID()
                    right = MyRandom.rInt()
                    sql = self.operator_sql.format("NULL:%d"%(left), right)
                    success, result = query(conn, sql)

                    if right == 0:
                        assert success == False
                        continue
                    
                    assert success == True
                    if right == 1:
                        continue
                    else:
                        assert result == None

            def test_markednull_constant_special_0(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):
                    left  = MyRandom.rNullID()
                    right = 0
                    sql = self.operator_sql.format("NULL:%d"%(left), right)
                    success, result = query(conn, sql)

                    assert success == False

            def test_markednull_constant_special_1(self, setup_clean):

                conn = setup_clean
                for _ in range(repitition):
                    left  = MyRandom.rNullID()
                    right = 1
                    sql = self.operator_sql.format("NULL:%d"%(left), right)
                    success, result = query(conn, sql)

                    assert success == True
                    assert result == "NULL:%d"%(left)


    class TestOperatorMod:

        class TestMarkedOperatorMarked:

            operator_sql = """
            SELECT
                '{0}'::INTT % '{1}'::INTT;
            """

            def test_constant_constant(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):

                    left  = MyRandom.rInt()
                    right = MyRandom.rInt()
                    sql = self.operator_sql.format(left, right)
                    success, result = query(conn, sql)

                    if  right == 0:
                        assert success == False
                        continue

                    if success == False:
                        print("""
                        \n
                        LEFT: {}, RIGHT: {},
                        ERROR: {},
                        """.format(left, right, result))

                    assert success == True
                    assert result == str(left - int(left / right) * right)

            def test_constant_markednull(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):
                    left  = MyRandom.rInt()
                    right = MyRandom.rNullID()
                    sql = self.operator_sql.format(left, "NULL:%d"%(right))
                    success, result = query(conn, sql)

                    assert success == True
                    if left == 0:
                        continue
                    else:
                        assert result == None

            def test_constant_markednull_special_0(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):
                    left  = 0
                    right = MyRandom.rNullID()
                    sql = self.operator_sql.format(left, "NULL:%d"%(right))
                    success, result = query(conn, sql)

                    assert success == True
                    assert result == '0'

            def test_markednull_constant(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):
                    left  = MyRandom.rNullID()
                    right = MyRandom.rInt()
                    sql = self.operator_sql.format("NULL:%d"%(left), right)
                    success, result = query(conn, sql)

                    if right == 0:
                        assert success == False
                        continue
                    
                    assert success == True
                    if right == 1:
                        continue
                    else:
                        assert result == None

            def test_markednull_constant_special_0(self, setup_clean):
                conn = setup_clean
                for _ in range(repitition):
                    left  = MyRandom.rNullID()
                    right = 0
                    sql = self.operator_sql.format("NULL:%d"%(left), right)
                    success, result = query(conn, sql)

                    assert success == False

            def test_markednull_constant_special_1(self, setup_clean):

                conn = setup_clean
                for _ in range(repitition):
                    left  = MyRandom.rNullID()
                    right = 1
                    sql = self.operator_sql.format("NULL:%d"%(left), right)
                    success, result = query(conn, sql)

                    assert success == True
                    assert result == '0'

            def test_markednull_constant_special_minus_1(self, setup_clean):

                conn = setup_clean
                for _ in range(repitition):
                    left  = MyRandom.rNullID()
                    right = -1
                    sql = self.operator_sql.format("NULL:%d"%(left), right)
                    success, result = query(conn, sql)

                    assert success == True
                    assert result == '0'


class TestNumericc:

    class TestIO:

        def test_marked_constant(self, setup_clean):
            conn = setup_clean
            for _ in range (repitition):
                number = MyRandom.rNumeric()
                sql = "SELECT '{}'::NUMERICC;".format(number)
                success, result = query(conn, sql)
                assert success == True
                assert float(result) == number
        
        def test_marked_null(self, setup_clean):
            conn = setup_clean
            for _ in range (repitition):
                id = "NULL:" + str(MyRandom.rNullID())
                sql = "SELECT '{}'::NUMERICC;".format(id)
                success, result = query(conn, sql)
                assert success == True
                assert result == id
        
        def test_sql_null(self, setup_clean):
            conn = setup_clean
            sql = "SELECT NULL::NUMERICC;"
            success, result = query(conn, sql)
            assert success == True
            assert result is None

        def test_illegal1(self, setup_clean):
            conn = setup_clean
            id = "NULL:" + str(MyRandom.rDate())
            sql = "SELECT '{}'::NUMERICC;".format(id)
            success, result = query(conn, sql)
            assert success == False
        
        def test_illegal3(self, setup_clean):
            conn = setup_clean
            id = "NULL:" + str(MyRandom.rText())
            sql = "SELECT '{}'::NUMERICC;".format(id)
            success, result = query(conn, sql)
            assert success == False
        
        def test_illegal4(self, setup_clean):
            conn = setup_clean
            id = "NULL:" + str(MyRandom.rWord())
            sql = "SELECT '{}'::NUMERICC;".format(id)
            success, result = query(conn, sql)
            assert success == False
        
        def test_illegal5(self, setup_clean):
            conn = setup_clean
            id = "NULL:" + str(0 - MyRandom.rNullID())
            sql = "SELECT '{}'::NUMERICC;".format(id)
            success, result = query(conn, sql)
            assert success == False
        
        def test_illegal6(self, setup_clean):
            conn = setup_clean
            sql = "SELECT 'null:'::NUMERICC;"
            success, result = query(conn, sql)
            assert success == False


class TestDatee:

    class TestIO:

        def test_marked_constant(self, setup_clean):
            conn = setup_clean
            for _ in range (repitition):
                date = MyRandom.rDate()
                sql = "SELECT '{}'::DATEE;".format(date)
                success, result = query(conn, sql)
                assert success == True

        def test_marked_constant2(self, setup_clean):
            conn = setup_clean
            for _ in range (repitition):
                date = MyRandom.rDateDDMMYY()
                sql = "SELECT '{}'::DATEE;".format(date)
                success, result = query(conn, sql)
                assert success == True
                assert result == date
        
        def test_marked_null(self, setup_clean):
            conn = setup_clean
            for _ in range (repitition):
                id = "NULL:" + str(MyRandom.rNullID())
                sql = "SELECT '{}'::DATEE;".format(id)
                success, result = query(conn, sql)
                assert success == True
                assert result == id
        
        def test_sql_null(self, setup_clean):
            conn = setup_clean
            sql = "SELECT NULL::DATEE;"
            success, result = query(conn, sql)
            assert success == True
            assert result is None

        def test_illegal1(self, setup_clean):
            conn = setup_clean
            id = "NULL:" + str(MyRandom.rNumeric())
            sql = "SELECT '{}'::DATEE;".format(id)
            success, result = query(conn, sql)
            assert success == False
        
        def test_illegal3(self, setup_clean):
            conn = setup_clean
            id = "NULL:" + str(MyRandom.rText())
            sql = "SELECT '{}'::DATEE;".format(id)
            success, result = query(conn, sql)
            assert success == False
        
        def test_illegal4(self, setup_clean):
            conn = setup_clean
            id = "NULL:" + str(MyRandom.rWord())
            sql = "SELECT '{}'::DATEE;".format(id)
            success, result = query(conn, sql)
            assert success == False
        
        def test_illegal5(self, setup_clean):
            conn = setup_clean
            id = "NULL:" + str(0 - MyRandom.rNullID())
            sql = "SELECT '{}'::DATEE;".format(id)
            success, result = query(conn, sql)
            assert success == False
        
        def test_illegal6(self, setup_clean):
            conn = setup_clean
            sql = "SELECT 'null:'::DATEE;"
            success, result = query(conn, sql)
            assert success == False


class TestTextt:

    class TestIO:

        def test_marked_constant(self, setup_clean):
            conn = setup_clean
            for _ in range (repitition):
                text = MyRandom.rText()
                sql = "SELECT '{}'::TEXTT;".format(text)
                success, result = query(conn, sql)
                assert success == True
                assert result == text.replace("\'\'", '\'')
        
        def test_marked_null(self, setup_clean):
            conn = setup_clean
            for _ in range (repitition):
                id = "NULL:" + str(MyRandom.rNullID())
                sql = "SELECT '{}'::TEXTT;".format(id)
                success, result = query(conn, sql)
                assert success == True
                assert result == id
        
        def test_sql_null(self, setup_clean):
            conn = setup_clean
            sql = "SELECT NULL::TEXTT;"
            success, result = query(conn, sql)
            assert success == True
            assert result is None
        
        def test_illegal4(self, setup_clean):
            conn = setup_clean
            id = "NULL:" + str(MyRandom.rWord())
            sql = "SELECT '{}'::TEXTT;".format(id)
            success, result = query(conn, sql)
            assert success == True
        
        def test_illegal5(self, setup_clean):
            conn = setup_clean
            id = "NULL:" + str(0 - MyRandom.rNullID())
            sql = "SELECT '{}'::TEXTT;".format(id)
            success, result = query(conn, sql)
            assert success == True
        
        def test_illegal6(self, setup_clean):
            conn = setup_clean
            sql = "SELECT 'null:'::TEXTT;"
            success, result = query(conn, sql)
            assert success == True


if __name__ == '__main__':
    pytest.main(['run.py', '-s'])
