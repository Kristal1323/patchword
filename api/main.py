import random
from pathlib import Path
from typing import List, Optional

from fastapi import FastAPI, HTTPException
from pydantic import BaseModel, Field
from fastapi.responses import FileResponse, HTMLResponse

DATA_PATH = Path(__file__).resolve().parents[1] / "data" / "words_alpha.txt"


def load_dictionary() -> List[str]:
    if not DATA_PATH.exists():
        raise FileNotFoundError(f"Dictionary not found at {DATA_PATH}")
    with DATA_PATH.open("r", encoding="utf-8") as f:
        words = [line.strip() for line in f if line.strip()]
    return words


DICT_WORDS = load_dictionary()
DICT_SET = {w.upper() for w in DICT_WORDS}

STARTER_LEN_DEFAULT = 3
VALID_STARTER_LEN = {3, 4}


def has_vowel(word: str) -> bool:
    vowels = set("AEIOUaeiou")
    return any(c in vowels for c in word)


def pick_starter(length: int) -> str:
    """
    Pick a starter substring of the given length from the dictionary.
    Prefers words with vowels and alphabetic characters only.
    """
    if length not in VALID_STARTER_LEN:
        raise ValueError("Starter length must be 3 or 4")

    candidates: List[str] = []
    for w in DICT_WORDS:
        if len(w) < length or not w.isalpha():
            continue
        for i in range(0, len(w) - length + 1):
            sub = w[i : i + length]
            if has_vowel(sub):
                candidates.append(sub.upper())
    if not candidates:
        return "ING" if length == 3 else "STAR"
    return random.choice(candidates)


def words_containing(subword: str) -> List[str]:
    sub = subword.upper()
    return [w for w in DICT_WORDS if sub in w.upper()]


class PlayRequest(BaseModel):
    starter: Optional[str] = Field(
        None, description="Starter subword (3-4 letters). If omitted, one is generated."
    )
    length: Optional[int] = Field(
        None, description="Starter length (3 or 4). Ignored if starter is provided."
    )
    guesses: List[str] = Field(..., description="Words guessed by the player.")


class PlayResult(BaseModel):
    starter: str
    valid_guesses: List[str]
    total_length: int
    longest_guesses: List[str]
    best_possible: List[str]


class SearchRequest(BaseModel):
    subwords: List[str] = Field(..., description="List of substrings to search for.")
    limit: int = Field(50, ge=1, le=200, description="Max results returned per subword.")


class SearchResult(BaseModel):
    subword: str
    count: int
    words: List[str]


app = FastAPI(title="Patchword API", version="0.1.0")
INDEX_PATH = Path(__file__).parent / "static" / "index.html"


@app.get("/health")
def health():
    return {"status": "ok", "dictionary_size": len(DICT_WORDS)}


@app.get("/", response_class=HTMLResponse)
def landing_page():
    if INDEX_PATH.exists():
        return FileResponse(INDEX_PATH)
    return HTMLResponse("<h1>Patchword API</h1><p>UI file not found.</p>")


@app.get("/starter")
def starter(length: int = STARTER_LEN_DEFAULT):
    if length not in VALID_STARTER_LEN:
        raise HTTPException(status_code=400, detail="Length must be 3 or 4.")
    return {"starter": pick_starter(length)}


@app.post("/play", response_model=PlayResult)
def play(req: PlayRequest):
    starter = req.starter.upper() if req.starter else None
    if starter:
        if len(starter) not in VALID_STARTER_LEN or not starter.isalpha():
            raise HTTPException(status_code=400, detail="Starter must be 3 or 4 letters.")
    else:
        use_len = req.length if req.length in VALID_STARTER_LEN else STARTER_LEN_DEFAULT
        starter = pick_starter(use_len)

    valid: List[str] = []
    seen = set()
    starter_up = starter.upper()

    for g in req.guesses:
        word = g.strip()
        if not word:
            continue
        word_up = word.upper()
        if (
            word_up.isalpha()
            and starter_up in word_up
            and word_up != starter_up
            and word_up in DICT_SET
            and word_up not in seen
        ):
            valid.append(word_up)
            seen.add(word_up)

    total_len = sum(len(w) for w in valid)
    longest_len = max((len(w) for w in valid), default=0)
    longest = [w for w in valid if len(w) == longest_len] if longest_len else []

    possible = words_containing(starter_up)
    best_len = max((len(w) for w in possible), default=0)
    best_possible = [w.upper() for w in possible if len(w) == best_len] if best_len else []

    return PlayResult(
        starter=starter_up,
        valid_guesses=valid,
        total_length=total_len,
        longest_guesses=longest,
        best_possible=best_possible[:20],
    )


@app.post("/search", response_model=List[SearchResult])
def search(req: SearchRequest):
    results: List[SearchResult] = []
    for sub in req.subwords:
        sub_clean = sub.strip()
        if not sub_clean or len(sub_clean) < 2:
            raise HTTPException(status_code=400, detail="Each subword must be at least 2 letters.")
        matches = words_containing(sub_clean)
        results.append(
            SearchResult(
                subword=sub_clean,
                count=len(matches),
                words=[w.upper() for w in matches[: req.limit]],
            )
        )
    return results
