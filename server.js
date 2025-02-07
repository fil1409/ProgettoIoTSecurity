const express = require('express');
const nodemailer = require('nodemailer');
const bodyParser = require('body-parser');
const app = express();
const port = 3000;

// Configurazione del trasportatore email
const transporter = nodemailer.createTransport({
    service: 'gmail',
    auth: {
        user: '',
        pass: ''
    }
});

// Database simulato degli utenti
const users = [
    {
        id: "1",
        nome: "",
        cognome: "",
        email: ""
    },
    {
        id: "2",
        nome: "",
        cognome: "",
        email: ""
    },
    {
        id: "3",
        nome: "",
        cognome: "",
        email: ""
    }
   
];

app.use(express.json());
app.use(bodyParser.urlencoded({ extended: true }));

// Endpoint per ottenere tutti gli utenti
app.get('/users', (req, res) => {
    res.json(users);
});

// Endpoint per ottenere un utente specifico per ID
app.get('/users/:id', (req, res) => {
    const user = users.find(u => u.id === req.params.id);
    if (user) {
        res.json(user);
    } else {
        res.status(404).json({ message: "Utente non trovato" });
    }
});

// Endpoint per l'invio delle email
app.post('/sendemail', async (req, res) => {
    try {
        const { email, name, otp } = req.body;

        // Verifica che tutti i campi necessari siano presenti
        if (!email || !name || !otp) {
            return res.status(400).json({ 
                message: "Mancano dei dati necessari (email, name, otp)" 
            });
        }

        // Configura l'email
        const mailOptions = {
            from: '',
            to: email,
            subject: 'Codice di Accesso',
            html: `
                <h2>Codice di Accesso al Sistema</h2>
                <p>Gentile ${name},</p>
                <p>Il tuo codice di accesso è: <strong>${otp}</strong></p>
                <p>Questo codice scadrà tra 5 minuti.</p>
                <p>Se non hai richiesto questo codice, ignora questa email.</p>
                <br>
                <p>Cordiali saluti.</p>
            `
        };

        // Invia l'email
        await transporter.sendMail(mailOptions);
        console.log(`Email inviata con successo a ${email}`);
        res.status(200).json({ message: 'Email inviata con successo' });

    } catch (error) {
        console.error('Errore nell\'invio dell\'email:', error);
        res.status(500).json({ 
            message: 'Errore nell\'invio dell\'email',
            error: error.message 
        });
    }
});

// Middleware per la gestione degli errori
app.use((err, req, res, next) => {
    console.error(err.stack);
    res.status(500).json({ 
        message: 'Si è verificato un errore interno',
        error: err.message 
    });
});


// Avvia il server
app.listen(port, '0.0.0.0', () => {
    console.log(`Server in esecuzione su http://localhost:${port}`);
    console.log('Server pronto per gestire le richieste email');
});