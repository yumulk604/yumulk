from __future__ import annotations

import json
import os
import shutil
import sqlite3
from contextlib import contextmanager
from dataclasses import asdict, dataclass
from datetime import datetime
from pathlib import Path
from typing import Any, Dict, Generator, Iterable, List


DEFAULT_DB_PATH = str(Path(os.environ.get("OFFLINESHOP_DB", "/workspace/offlineshop.db")))


def _ensure_parent_dir(path: Path) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)


def get_connection(db_path: Path | str) -> sqlite3.Connection:
    connection = sqlite3.connect(str(db_path))
    connection.row_factory = sqlite3.Row
    connection.execute("PRAGMA foreign_keys = ON")
    return connection


@contextmanager
def db_cursor(db_path: Path | str) -> Generator[sqlite3.Cursor, None, None]:
    conn = get_connection(db_path)
    try:
        cur = conn.cursor()
        yield cur
        conn.commit()
    finally:
        conn.close()


def initialize_database(db_path: Path | str = DEFAULT_DB_PATH) -> None:
    schema_file = Path(__file__).with_name("schema.sql")
    with get_connection(db_path) as conn:
        with open(schema_file, "r", encoding="utf-8") as f:
            conn.executescript(f.read())


def backup_database(db_path: Path | str, out_dir: Path | str) -> Path:
    out_dir = Path(out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    db_path = Path(db_path)
    backup_path = out_dir / f"offlineshop_backup_{timestamp}.db"
    shutil.copy2(db_path, backup_path)
    return backup_path


def export_database_to_json(db_path: Path | str, out_file: Path | str) -> None:
    out_file = Path(out_file)
    _ensure_parent_dir(out_file)
    with get_connection(db_path) as conn:
        cursor = conn.cursor()
        tables = [
            "products",
            "customers",
            "sales",
            "sale_items",
        ]
        data: Dict[str, List[Dict[str, Any]]] = {}
        for table in tables:
            rows = cursor.execute(f"SELECT * FROM {table}").fetchall()
            data[table] = [dict(row) for row in rows]
    with open(out_file, "w", encoding="utf-8") as f:
        json.dump(data, f, indent=2, ensure_ascii=False)


def import_database_from_json(db_path: Path | str, in_file: Path | str) -> None:
    with open(in_file, "r", encoding="utf-8") as f:
        data = json.load(f)
    with get_connection(db_path) as conn:
        cursor = conn.cursor()
        cursor.execute("PRAGMA foreign_keys = OFF")
        try:
            # Clear existing data
            cursor.execute("DELETE FROM sale_items")
            cursor.execute("DELETE FROM sales")
            cursor.execute("DELETE FROM products")
            cursor.execute("DELETE FROM customers")
            # Insert new data
            for row in data.get("products", []):
                columns = ",".join(row.keys())
                placeholders = ",".join([":" + k for k in row.keys()])
                cursor.execute(
                    f"INSERT OR REPLACE INTO products ({columns}) VALUES ({placeholders})",
                    row,
                )
            for row in data.get("customers", []):
                columns = ",".join(row.keys())
                placeholders = ",".join([":" + k for k in row.keys()])
                cursor.execute(
                    f"INSERT OR REPLACE INTO customers ({columns}) VALUES ({placeholders})",
                    row,
                )
            for row in data.get("sales", []):
                columns = ",".join(row.keys())
                placeholders = ",".join([":" + k for k in row.keys()])
                cursor.execute(
                    f"INSERT OR REPLACE INTO sales ({columns}) VALUES ({placeholders})",
                    row,
                )
            for row in data.get("sale_items", []):
                columns = ",".join(row.keys())
                placeholders = ",".join([":" + k for k in row.keys()])
                cursor.execute(
                    f"INSERT OR REPLACE INTO sale_items ({columns}) VALUES ({placeholders})",
                    row,
                )
        finally:
            cursor.execute("PRAGMA foreign_keys = ON")

