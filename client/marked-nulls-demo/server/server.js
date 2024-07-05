const express = require('express');
const { Pool } = require('pg');
const cors = require('cors');
const bodyParser = require('body-parser');

const app = express();
const port = process.env.PORT || 5000;

app.use(cors());
app.use(bodyParser.json());

const pool = new Pool({
  user: 'postgres',
  password: 'root',
  host: 'localhost',
  port: 5432,
  database: 'usability',
});

app.post('/api/execute-query', (req, res) => {
  const { query } = req.body;

  // Use a regular expression to extract parameter placeholders from the query
  const paramRegex = /\$(\d+)/g;
  const params = [];

  // Replace parameter placeholders with $1, $2, etc.
  const sanitizedQuery = query.replace(paramRegex, (match, paramIndex) => {
    params.push(paramIndex);
    return `$${params.length}`;
  });

  pool.query(sanitizedQuery, params, (err, result) => {
    if (err) {
      console.error('Error executing query:', err);
      return res.status(500).json({ error: 'An error occurred while executing the query' });
    }
    
    if (result.command === 'SELECT') {
      // Select query
      res.json(result.rows);
    } else if (result.command === 'INSERT' || result.command === 'UPDATE' || result.command === 'DELETE') {
      // Insert, update, or delete query
      res.json({ rowCount: result.rowCount, command: result.command });
    } else if (result.command === 'CREATE' || result.command === 'DROP' || result.command === 'ALTER') {
      // Create, drop, or alter query
      res.json({ message: 'Query executed successfully', command: result.command });
    } else {
      // Other types of queries
      res.json({ message: 'Query executed successfully' });
    }
  });
});

// API endpoint to fetch table schema
app.get('/api/table-schema', async (req, res) => {
  try {
    const client = await pool.connect();
    const result = await client.query(`
      SELECT table_name, column_name, data_type
      FROM information_schema.columns
      WHERE table_schema = 'public'
      ORDER BY table_name, ordinal_position;
    `);
    client.release();
    res.json(result.rows);
  } catch (error) {
    console.error('Error fetching table schema:', error);
    res.status(500).json({ error: 'Internal server error' });
  }
});

app.listen(port, () => {
  console.log(`Server is running on port ${port}`);
});