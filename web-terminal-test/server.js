const express = require('express');
const path = require('path');
const http = require('http');
const WebSocket = require('ws');
const pty = require('node-pty');
const passport = require('passport');
const LocalStrategy = require('passport-local').Strategy;
const session = require('express-session');
const bcrypt = require('bcrypt');

// Dummy users database
const users = [{ id: 1, username: 'user', password: bcrypt.hashSync('password', 10) }];

passport.use(new LocalStrategy((username, password, done) => {
  const user = users.find(u => u.username === username);
  if (!user) return done(null, false, { message: 'Incorrect username.' });
  if (!bcrypt.compareSync(password, user.password)) return done(null, false, { message: 'Incorrect password.' });
  return done(null, user);
}));

passport.serializeUser((user, done) => done(null, user.id));
passport.deserializeUser((id, done) => {
  const user = users.find(u => u.id === id);
  done(null, user);
});

const app = express();
app.use(session({ secret: 'secret', resave: false, saveUninitialized: false }));
app.use(passport.initialize());
app.use(passport.session());
app.use(express.static(path.join(__dirname, 'public')));

app.post('/login', passport.authenticate('local', {
  successRedirect: '/',
  failureRedirect: '/login',
}));

app.get('/login', (req, res) => {
  res.sendFile(path.join(__dirname, 'public', 'login.html'));
});

app.get('/', (req, res, next) => {
  if (req.isAuthenticated()) return next();
  res.redirect('/login');
}, (req, res) => {
  res.sendFile(path.join(__dirname, 'public', 'index.html'));
});

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

