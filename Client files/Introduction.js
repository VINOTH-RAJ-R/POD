import React from 'react';
import './Introduction.css';

function Introduction() {
  return (
    <div className="introduction">
      <header className="header">
        <h1>Introduction to Marked Nulls in PostgreSQL</h1>
        <p>Enhance your database with Marked Nulls</p>
      </header>

      <section className="section">
        <h2>Existing Nulls in PostgreSQL</h2>
        <p>In PostgreSQL, the <code>null</code> value can be ambiguous. SQL's evaluation procedure uses three-valued logic where the result of logical operations can be true, false, or unknown. The unknown state is often caused by the presence of null values.</p>
        <p>Null values can be interpreted to mean different things:</p>
        <ul>
          <li>A value exists but is missing (missing-but-applicable)</li>
          <li>No value exists or a value does not make sense (missing-and-inapplicable)</li>
        </ul>
        <p>Consider the following example:</p>
        <pre className="code-block">
          <code>
{`CREATE TABLE Person (
  id int primary key,
  name varchar(20),
  age int,
  salary numeric(10, 2)
);`}
          </code>
        </pre>
        <table className="table">
          <thead>
            <tr>
              <th>ID</th>
              <th>Name</th>
              <th>Age</th>
              <th>Salary</th>
            </tr>
          </thead>
          <tbody>
            <tr>
              <td>1</td>
              <td>John</td>
              <td>30</td>
              <td>50000.00</td>
            </tr>
            <tr>
              <td>2</td>
              <td>Jane</td>
              <td>null</td>
              <td>60000.00</td>
            </tr>
            <tr>
              <td>3</td>
              <td>Mike</td>
              <td>25</td>
              <td>null</td>
            </tr>
            <tr>
              <td>4</td>
              <td>Emily</td>
              <td>null</td>
              <td>null</td>
            </tr>
          </tbody>
        </table>
        <p>In this example:</p>
        <ul>
          <li>Jane's age is null: It could mean we don't know her age, but she certainly has one.</li>
          <li>Mike's salary is null: It could mean he's unemployed (missing-and-inapplicable) or we just don't know his salary (missing-but-applicable).</li>
          <li>Emily's age and salary are both null: We can't distinguish between unknown values and inapplicable values.</li>
        </ul>
      </section>

      <section className="section">
        <h2>Problems with Existing Nulls</h2>
        <p>The ambiguity of null values can lead to unexpected query results. Consider this query:</p>
        <pre className="code-block">
          <code>
            SELECT * FROM Person WHERE age {'>'}= 18 OR age {'<'} 18;
          </code>
        </pre>
        <p>Logically, this query should return all persons, as everyone's age is either greater than or equal to 18, or less than 18. However, persons with null ages will not be included in the result set because comparisons with null return unknown, which is treated as false in the WHERE clause.</p>
        <p>Another problematic example:</p>
        <pre className="code-block">
          <code>
{`SELECT name FROM Person WHERE
NOT name IN
(SELECT name FROM Person WHERE age >= 18 OR age < 18);`}
          </code>
        </pre>
        <p>This query asks for the names of people who do not have an age greater than or equal to 18 OR less than 18, which should logically return no one. However, it will return the names of all people with null ages, which is incorrect.</p>
      </section>

      <section className="section">
        <h2>Introducing Marked Nulls</h2>
        <p>Marked Nulls address these limitations by allowing us to distinguish between unknown values and non-existent values, and to identify relationships between unknown values.</p>
        <p>With Marked Nulls, we introduce new data types, along with the existing data types:</p>
        <ul>
          <li><code>intt</code>: for integers</li>
          <li><code>numericc</code>: for numeric values</li>
          <li><code>textt</code>: for text</li>
          <li><code>datee</code>: for dates</li>
        </ul>
        <p>Here's how we would create our employees table using Marked Nulls:</p>
        <pre className="code-block">
          <code>
{`CREATE TABLE employees_marked (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    department VARCHAR(50) NOT NULL,
    salary intt,  -- Marked Null version of INTEGER
    phone_number textt,  -- Marked Null version of VARCHAR
    emergency_contact textt
);`}
          </code>
        </pre>
        <p>Note how we use <code>intt</code> for the salary column and <code>textt</code> for phone_number and emergency_contact. These types allow for Marked Null values.</p>
        
        <h3>Inserting Data with Marked Nulls</h3>
        <p>When inserting data with Marked Nulls, it's important to use the correct syntax. Here are some examples:</p>
        <pre className="code-block">
          <code>
{`INSERT INTO employees_marked (name, department, salary, phone_number, emergency_contact) VALUES
('John Doe', 'Sales', 50000, '123-456-7890', 'Jane Doe'),
('Jane Smith', 'Marketing', 55000, NULL, 'John Smith'),
('Bob Johnson', 'IT', 'NULL:1', '987-654-3210', NULL),
('Alice Brown', 'HR', 52000, 'NULL:2', 'NULL:a'),
('Charlie Wilson', 'Sales', 48000, '555-123-4567', 'Sarah Wilson'),
('David Lee', 'IT', 'NULL:1', 'NULL:2', 'NULL:b'),
('Mark Walls', 'HR', 55000, '333-444-5555', 'John Doe');`}
          </code>
        </pre>
        <p>Note that:</p>
        <ul>
          <li>Marked Nulls are inserted as strings with single quotes, e.g., 'NULL:1', 'NULL:a'.</li>
          <li>Regular NULLs are inserted without quotes.</li>
          <li>You can use numbers (NULL:1, NULL:2) or letters (NULL:a, NULL:b) as identifiers.</li>
        </ul>

        <h3>Querying with Marked Nulls</h3>
        <p>Marked Nulls allow for more nuanced queries. Here's an example query to find employees with the same salary:</p>
        <pre className="code-block">
          <code>
{`SELECT 
    a.name AS employee1, 
    b.name AS employee2, 
    a.salary
FROM 
    employees_marked a
JOIN 
    employees_marked b ON a.id < b.id
WHERE 
    a.salary = b.salary;`}
          </code>
        </pre>
        <p>This query might return results like:</p>
        <table className="table">
          <thead>
            <tr>
              <th>employee1</th>
              <th>employee2</th>
              <th>salary</th>
            </tr>
          </thead>
          <tbody>
            <tr>
              <td>Jane Smith</td>
              <td>Mark Walls</td>
              <td>55000</td>
            </tr>
            <tr>
              <td>Bob Johnson</td>
              <td>David Lee</td>
              <td>NULL:1</td>
            </tr>
          </tbody>
        </table>
        <p>Notice how this query can match both regular salaries and Marked Nulls with the same identifier. This demonstrates the power of Marked Nulls in allowing us to identify relationships between unknown values that would not be possible with traditional NULL values.</p>
      </section>

      <section className="section cta-section">
        <h2>Try Marked Nulls</h2>
        <p>Now, it's your turn to explore Marked Nulls! We provide two options for you to test and play with this new feature:</p>
        <div className="cta-buttons">
          <a href="/workspace?env=cli" target="_blank" rel="noopener noreferrer" className="cta-button">CLI-based Environment</a>
          <a href="/workspace?env=ui" target="_blank" rel="noopener noreferrer" className="cta-button">Web UI-based Environment</a>
        </div>
        <p>Please note that these are only different environments for you to work on based on your preference. The implementation will work the same way in both of them.</p>
        <p>Each option will open in a new tab, and you'll have a separate instance of the database to work with. Feel free to experiment and explore the capabilities of Marked Nulls!</p>
      </section>
    </div>
  );
}

export default Introduction;