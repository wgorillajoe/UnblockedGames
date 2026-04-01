# Discord Bot Guild Leaver (Safe Version)

This project is a **safe alternative** to user-token/self-bot server leaver scripts.
It only works with a **Discord bot token**.

## Step-by-step (Windows)
1. Install Python 3.10+ from https://www.python.org/downloads/.
2. Put these files together in one folder:
   - `main.py`
   - `requirements.txt`
   - `run_leaver.bat`
3. Double-click `run_leaver.bat`.
4. On first run, the script creates:
   - `token.txt`
   - `exceptions.txt`
5. Open `token.txt`, replace `PASTE_YOUR_BOT_TOKEN_HERE` with your real bot token, and save.
6. (Optional) Open `exceptions.txt` and add guild IDs you **do not** want the bot to leave (one per line).
7. Double-click `run_leaver.bat` again.
8. Watch the console output for leave/skip/fail results.

## Step-by-step (macOS/Linux)
1. Open terminal in this folder.
2. Install dependencies:
   ```bash
   pip install -r requirements.txt
   ```
3. Run once to create template files:
   ```bash
   python3 main.py
   ```
4. Edit `token.txt` and paste your bot token.
5. (Optional) Edit `exceptions.txt` and add keep-guild IDs.
6. Run again:
   ```bash
   python3 main.py
   ```


## How to get your bot token
1. Go to the Discord Developer Portal: https://discord.com/developers/applications
2. Create an application (or open your existing one).
3. Open **Bot** in the left sidebar.
4. If needed, click **Add Bot**.
5. In the bot page, click **Reset Token** (or **Copy** if visible).
6. Paste that value into `token.txt` and save.

### Important safety notes
- Never share your bot token publicly.
- If you accidentally leak it, immediately reset/regenerate it in the portal.
- This script is for your own bot account only (not user account tokens).

## Notes
- The script leaves guilds one-by-one with a small delay to reduce API pressure.
- If an ID in `exceptions.txt` is invalid, it is skipped with a warning.
- Bots cannot leave guilds they are not in.
