# patchword
CLI word game for the user to use the given sub-word and make guesses for the longest word possible. 

## Web/API wrapper
You can expose the game and subword search as an HTTP service without touching the C code:

1. Install deps (one time):
   ```bash
   cd patchword/api
   python -m venv .venv && source .venv/bin/activate
   pip install -r requirements.txt
   ```

2. Run the API (serves on http://localhost:8000):
   ```bash
   uvicorn main:app --reload --host 0.0.0.0 --port 8000
   ```

3. Endpoints:
   - `GET /health` – dictionary status.
   - `POST /play` – body `{ "starter": "cat", "length": 3|4, "guesses": ["catalog", ...] }` (starter optional; length used when starter omitted).
   - `POST /search` – body `{ "subwords": ["ing", "art"], "limit": 50 }` (returns counts + top matches per subword).

Deploy the API in a container (add this folder to your image, run uvicorn) and pair with a small web UI if desired.

### Web UI
- A simple UI is bundled at `api/static/index.html` and served by the API root (`GET /`).
- Start the API (`uvicorn main:app --host 0.0.0.0 --port 8000`) then open `http://localhost:8000` to play or run subword searches.
