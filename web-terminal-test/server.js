const express = require('express');
const path = require('path');
const http = require('http');
const WebSocket = require('ws');
const pty = require('node-pty');
const session = require('express-session');

const app = express();
app.use(session({ secret: 'secret', resave: false, saveUninitialized: false }));

app.use(express.static(path.join(__dirname, 'public')));

const server = http.createServer(app);
const wss = new WebSocket.Server({ server });

wss.on('connection', ws => {
  const shell = process.env.SHELL || 'bash';
  const ptyProcess = pty.spawn(shell, [], {
    name: 'xterm-color',
    cols: 80,
    rows: 24,
    cwd: process.env.HOME,
    env: process.env
  });

  // List of initial commands to execute
  const initialCommands = [
    'docker run -it fedorapostgres /bin/bash',
    'su - postgres',
    '/usr/pgsql-16/bin/pg_ctl initdb',
    '/usr/pgsql-16/bin/pg_ctl -D /var/lib/pgsql/16/data/ -l logfile start',
    'exit',
    'export PATH=$PATH:/usr/pgsql-16/bin/',
    'cd /usr/local/src/PostgresSQL-Core-C/',
    'make',
    'make install',
    'clear',
    'psql -U postgres',
    'create extension markednullcore;',
    'CREATE TABLE employees_marked (id SERIAL PRIMARY KEY, name VARCHAR(100) NOT NULL, department VARCHAR(50) NOT NULL, salary int, phone_number text, emergency_contact text);',
    "INSERT INTO employees_marked (name, department, salary, phone_number, emergency_contact) VALUES ('John Doe', 'Sales', 50000, '123-456-7890', 'Jane Doe'),('Jane Smith', 'Marketing', 55000, NULL, 'John Smith'),('Bob Johnson', 'IT', NULL, '987-654-3210', NULL),('Alice Brown', 'HR', 52000, NULL, NULL),('Charlie Wilson', 'Sales', 48000, '555-123-4567', 'Sarah Wilson'),('David Lee', 'IT', NULL, NULL, NULL),('Mark Walls', 'HR', 55000, '333-444-5555', 'John Doe');",
    '\\q',
    'clear',
    'psql -U postgres',
];


  // Execute initial commands
  initialCommands.forEach(cmd => {
    ptyProcess.write(cmd + '\r');
  });

  ptyProcess.on('data', data => {
    ws.send(data);
  });

  ws.on('message', message => {
    ptyProcess.write(message);
  });

  ws.on('close', () => {
    ptyProcess.kill();
  });
});

server.listen(8080, () => {
  console.log('Server is listening on http://localhost:8080');
});
