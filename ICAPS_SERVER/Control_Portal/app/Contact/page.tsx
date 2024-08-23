import type React from 'react';

const ContactPage: React.FC = () => {
  return (
    <div style={styles.container}>
      <h1>Contact Us</h1>
      <form style={styles.form}>
        <div style={styles.formGroup}>
          <label htmlFor="name">Name:</label>
          <input type="text" id="name" name="name" required />
        </div>
        <div style={styles.formGroup}>
          <label htmlFor="email">Email:</label>
          <input type="email" id="email" name="email" required />
        </div>
        <div style={styles.formGroup}>
          <label htmlFor="subject">Subject:</label>
          <input type="text" id="subject" name="subject" required />
        </div>
        <div style={styles.formGroup}>
          <label htmlFor="message">Message:</label>
          <textarea id="message" name="message" required />
        </div>
        <button type="submit" style={styles.button}>Submit</button>
      </form>
    </div>
  );
};

const styles = {
  container: {
    maxWidth: '600px',
    margin: '0 auto',
    padding: '20px',
    textAlign: 'center' as const,
  },
  form: {
    display: 'flex',
    flexDirection: 'column' as const,
  },
  formGroup: {
    marginBottom: '15px',
  },
  button: {
    padding: '10px 20px',
    backgroundColor: '#007BFF',
    color: '#FFF',
    border: 'none',
    borderRadius: '5px',
    cursor: 'pointer',
  },
};

export default ContactPage;