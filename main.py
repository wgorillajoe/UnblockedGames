#!/usr/bin/env python3
"""Discord bot guild leaver (safe alternative to self-bot/user-token scripts).

Usage:
  1) Put your bot token in token.txt
  2) Put guild IDs to keep in exceptions.txt (one ID per line)
  3) Run: python3 main.py
"""

from __future__ import annotations

import asyncio
from pathlib import Path

import discord

TOKEN_FILE = Path("token.txt")
EXCEPTIONS_FILE = Path("exceptions.txt")
RATE_DELAY_SECONDS = 1.2


def load_token(path: Path) -> str:
    if not path.exists():
        raise FileNotFoundError(
            f"Missing {path}. Create it and paste your Discord bot token."
        )
    token = path.read_text(encoding="utf-8").strip()
    if not token:
        raise ValueError(f"{path} is empty. Paste your bot token into it.")
    return token


def load_exception_ids(path: Path) -> set[int]:
    if not path.exists():
        return set()

    keep: set[int] = set()
    for line in path.read_text(encoding="utf-8").splitlines():
        text = line.strip()
        if not text or text.startswith("#"):
            continue
        try:
            keep.add(int(text))
        except ValueError:
            print(f"[warn] Skipping invalid guild ID in {path}: {text!r}")
    return keep


class LeaveBot(discord.Client):
    def __init__(self, keep_ids: set[int], **kwargs):
        super().__init__(**kwargs)
        self.keep_ids = keep_ids

    async def on_ready(self) -> None:
        print(f"Logged in as {self.user} (id={self.user.id})")
        print(f"Connected guilds: {len(self.guilds)}")

        left = 0
        skipped = 0
        failed = 0

        for guild in list(self.guilds):
            if guild.id in self.keep_ids:
                skipped += 1
                print(f"[skip] Keeping guild {guild.name} ({guild.id})")
                continue

            try:
                print(f"[leave] Leaving guild {guild.name} ({guild.id})")
                await guild.leave()
                left += 1
            except discord.HTTPException as exc:
                failed += 1
                print(f"[fail] Could not leave {guild.name} ({guild.id}): {exc}")

            await asyncio.sleep(RATE_DELAY_SECONDS)

        print("\nDone.")
        print(f"Left: {left} | Skipped: {skipped} | Failed: {failed}")
        await self.close()


async def run() -> None:
    token = load_token(TOKEN_FILE)
    keep_ids = load_exception_ids(EXCEPTIONS_FILE)

    intents = discord.Intents.none()
    client = LeaveBot(keep_ids=keep_ids, intents=intents)
    await client.start(token)


if __name__ == "__main__":
    try:
        asyncio.run(run())
    except KeyboardInterrupt:
        print("Interrupted by user.")
