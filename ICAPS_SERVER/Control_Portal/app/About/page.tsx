import type React from 'react';

const AboutPage: React.FC = () => {
  return (
    <div style={styles.container}>
      <h1 className='text-3xl mb-4'>About Us</h1>
      <p>
        Welcome to our research project site. This project is on .
      </p>

    </div>
  );
};

const styles = {
  container: {
    maxWidth: '800px',
    margin: '0 auto',
    padding: '20px',
    textAlign: 'center' as const,
  },
};

export default AboutPage;