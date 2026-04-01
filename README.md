# Discord Bot Guild Leaver (Safe Version)

This project is a **safe alternative** to user-token/self-bot server leaver scripts.
It only works with a **Discord bot token**.

## Why this version
Using user account tokens in scripts is risky and can violate Discord Terms. This script is for bot accounts only.

## Install
1. Create and activate a Python environment (optional but recommended).
2. Install dependencies:
   ```bash
   pip install -r requirements.txt
   ```
3. Put your bot token in `token.txt`.
4. (Optional) Put guild IDs you want to keep in `exceptions.txt` (one ID per line).

## Run
```bash
python3 main.py
```

## Notes
- The script leaves guilds one-by-one with a small delay to reduce API pressure.
- If an ID in `exceptions.txt` is invalid, it is skipped with a warning.
- Bots cannot leave guilds they are not in.
