from flask import Flask, render_template, request, jsonify
import subprocess
import json
import os
import sqlite3

app = Flask(__name__)
DB_PATH = 'contacts.db'

def init_db(): #Create database table if it doesn't exist
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    cursor.execute('''
        CREATE TABLE IF NOT EXISTS contacts (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT UNIQUE NOT NULL,
            phone TEXT,
            email TEXT
        )
    ''')
    conn.commit()
    conn.close()

def load_contacts_into_tree(): # Database → AVL Tree
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    cursor.execute("SELECT name, phone, email FROM contacts")
    rows = cursor.fetchall()
    conn.close()
    for row in rows:
        send_command('cm', f'INSERT {sanitize(row[0])} {sanitize(row[1])} {sanitize(row[2])}')
        #rebuilt in memory.(State Reconstruction)

processes = {} #Instead of starting bst.exe every time the user clicks Insert, Flask starts it once and keeps it running.

def get_process(tree_type):
    global processes
    binary_ext = ".exe" if os.name == 'nt' else ""
    binary_path = f"./{tree_type}"
    if tree_type not in processes or processes[tree_type].poll() is not None: #Start process if: 1. It doesn't exist ,OR ,2. It crashed
        processes[tree_type] = subprocess.Popen(
            [binary_path],
            stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
            text=True, bufsize=1, shell=False
        )
    return processes[tree_type]

def send_command(tree_type, command):
    try:
        proc = get_process(tree_type)
        proc.stdin.write(command + '\n')
        proc.stdin.flush()
        return proc.stdout.readline().strip()
    except Exception as e:
        return json.dumps({"error": str(e)})

def sanitize(s):
    return str(s).replace(' ', '_')

def parse_response(response):
    if not response or response == 'null': return None
    try: return json.loads(response)
    except: return None

# --- ROUTING ENDPOINTS ---
@app.route('/')
def index():
    return render_template('index.html')

@app.route('/tree/<tree_type>')
def view_tree(tree_type):
    return render_template('visualizer.html', tree_type=tree_type)

@app.route('/contact-system')
def contact_system():
    return render_template('contacts.html')

# --- VISUALIZER ENGINE PATHWAYS ---
@app.route("/api/<tree_type>/insert", methods=["POST"])
def insert_node(tree_type):
    value = request.json.get("value")
    response = send_command(tree_type, f"INSERT {value}")
    return jsonify({"tree": parse_response(response), "message": f"Inserted {value}"})

@app.route("/api/<tree_type>/delete", methods=["POST"])
def delete_node(tree_type):
    value = request.json.get("value")
    response = send_command(tree_type, f"DELETE {value}")
    return jsonify({"tree": parse_response(response), "message": f"Deleted {value}"})

@app.route("/api/<tree_type>/search", methods=["POST"])
def search_node(tree_type):
    value = request.json.get("value")
    response = send_command(tree_type, f"SEARCH {value}")
    return jsonify({"found": response == "true", "message": "Search processed"})

@app.route("/api/<tree_type>/get", methods=["GET"])
def get_tree(tree_type):
    return jsonify({"tree": parse_response(send_command(tree_type, "GET"))})

@app.route("/api/<tree_type>/reset", methods=["POST"])
def reset_tree(tree_type):
    if tree_type in processes:
        processes[tree_type].terminate()
        del processes[tree_type]
    return jsonify({"status": "reset"})

# --- INTEGRATED CONTACT ENGINE PATHWAYS ---
@app.route('/api/cm/insert', methods=['POST'])
def cm_insert():
    data = request.json
    name, phone, email = data.get('name', ''), data.get('phone', 'N/A'), data.get('email', 'N/A')
    if not name: return jsonify({'error': 'Name missing'}), 400
    try:
        conn = sqlite3.connect(DB_PATH)
        cursor = conn.cursor()
        cursor.execute("INSERT INTO contacts (name, phone, email) VALUES (?, ?, ?)", (name, phone, email))
        conn.commit()
        conn.close()
        response = send_command('cm', f'INSERT {sanitize(name)} {sanitize(phone)} {sanitize(email)}')
        return jsonify({'tree': parse_response(response), 'message': f'Contact saved successfully.'})
    except sqlite3.IntegrityError:
        return jsonify({'error': 'Contact already exists'}), 400

@app.route('/api/cm/delete', methods=['POST'])
def cm_delete():
    name = request.json.get('name', '')
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    cursor.execute("DELETE FROM contacts WHERE name = ?", (name,))
    conn.commit()
    conn.close()
    response = send_command('cm', f'DELETE {sanitize(name)}')
    return jsonify({'tree': parse_response(response), 'message': f'Deleted contact successfully.'})

@app.route('/api/cm/search', methods=['POST'])
def cm_search():
    field = request.json.get('field', 'name')
    query = request.json.get('query', '')
    response = send_command('cm', f'SEARCH_{field.upper()} {sanitize(query)}')
    parsed = parse_response(response)
    return jsonify({'found': parsed is not None, 'contact': parsed})

@app.route('/api/cm/update', methods=['POST'])
def cm_update():
    data = request.json
    old_name, name = data.get('oldName', ''), data.get('name', '')
    phone, email = data.get('phone', 'N/A'), data.get('email', 'N/A')
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    cursor.execute("UPDATE contacts SET name=?, phone=?, email=? WHERE name=?", (name, phone, email, old_name))
    conn.commit()
    conn.close()
    response = send_command('cm', f'UPDATE {sanitize(old_name)} {sanitize(name)} {sanitize(phone)} {sanitize(email)}')
    return jsonify({'tree': parse_response(response), 'message': 'Updated contact.'})

@app.route('/api/cm/reset', methods=['POST'])
def cm_reset():
    send_command('cm', 'CLEAR_LOG')
    if 'cm' in processes:
        processes['cm'].terminate()
        del processes['cm']
    if os.path.exists(DB_PATH):
        os.remove(DB_PATH)
    init_db()
    return jsonify({"status": "reset"})

@app.route('/api/cm/list', methods=['GET'])
def cm_list():
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    cursor.execute("SELECT name, phone, email FROM contacts ORDER BY name ASC")
    contacts = [{'name': r[0], 'phone': r[1], 'email': r[2]} for r in cursor.fetchall()]
    conn.close()
    return jsonify({'contacts': contacts})

@app.route('/api/cm/tree', methods=['GET'])
def cm_tree():
    return jsonify({'tree': parse_response(send_command('cm', 'GET'))})

if __name__ == '__main__':
    init_db()
    try:
        get_process('cm')
        load_contacts_into_tree()
    except Exception as e:
        print(f"Subprocess boot issue: {e}")
    app.run(debug=True, port=5000)
