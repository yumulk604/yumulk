#!/usr/bin/env python3
import argparse
import json
import sys
from pathlib import Path

from offlineshop.db import (
    DEFAULT_DB_PATH,
    initialize_database,
    backup_database,
    export_database_to_json,
    import_database_from_json,
)
from offlineshop.repositories.product_repository import ProductRepository
from offlineshop.repositories.customer_repository import CustomerRepository
from offlineshop.services.sales_service import SalesService


def get_db_path(cli_value: str | None) -> Path:
    if cli_value:
        return Path(cli_value).expanduser().resolve()
    return Path(DEFAULT_DB_PATH)


def cmd_db(args: argparse.Namespace) -> None:
    db_path = get_db_path(args.db)
    if args.action == "init":
        initialize_database(db_path)
        print(f"Initialized database at {db_path}")
    elif args.action == "backup":
        out_dir = Path(args.out).expanduser().resolve()
        out_dir.mkdir(parents=True, exist_ok=True)
        backup_file = backup_database(db_path, out_dir)
        print(f"Backup created: {backup_file}")
    elif args.action == "export-json":
        out_file = Path(args.out).expanduser().resolve()
        export_database_to_json(db_path, out_file)
        print(f"Exported to {out_file}")
    elif args.action == "import-json":
        in_file = Path(args.file).expanduser().resolve()
        import_database_from_json(db_path, in_file)
        print(f"Imported from {in_file}")


def cmd_product(args: argparse.Namespace) -> None:
    db_path = get_db_path(args.db)
    repo = ProductRepository(db_path)
    if args.action == "add":
        product_id = repo.create_product(
            name=args.name,
            sku=args.sku,
            price=args.price,
            stock=args.stock,
        )
        print(f"Created product #{product_id}")
    elif args.action == "list":
        products = repo.list_products(search=args.search, low_at_or_below=args.low)
        if not products:
            print("No products found")
            return
        for p in products:
            print(
                f"#{p['id']} | {p['name']} | SKU:{p['sku']} | "
                f"Price:{p['price']:.2f} | Stock:{p['stock']}"
            )
    elif args.action == "update":
        updated = repo.update_product(
            product_id=args.id,
            name=args.name,
            sku=args.sku,
            price=args.price,
        )
        print("Updated" if updated else "No changes or product not found")
    elif args.action == "delete":
        deleted = repo.delete_product(product_id=args.id)
        print("Deleted" if deleted else "Product not found")
    elif args.action == "adjust-stock":
        ok = repo.adjust_stock(product_id=args.id, delta=args.delta)
        print("Stock updated" if ok else "Product not found")


def cmd_customer(args: argparse.Namespace) -> None:
    db_path = get_db_path(args.db)
    repo = CustomerRepository(db_path)
    if args.action == "add":
        customer_id = repo.create_customer(
            name=args.name, phone=args.phone, email=args.email
        )
        print(f"Created customer #{customer_id}")
    elif args.action == "list":
        customers = repo.list_customers(search=args.search)
        if not customers:
            print("No customers found")
            return
        for c in customers:
            print(
                f"#{c['id']} | {c['name']} | Phone:{c['phone'] or '-'} | "
                f"Email:{c['email'] or '-'}"
            )


def cmd_sale(args: argparse.Namespace) -> None:
    db_path = get_db_path(args.db)
    service = SalesService(db_path)
    if args.action == "create":
        try:
            items = json.loads(args.items)
            if not isinstance(items, list):
                raise ValueError
        except Exception:
            print(
                "Invalid --items. Provide JSON list of {product_id, qty}.",
                file=sys.stderr,
            )
            sys.exit(2)
        sale_id, total = service.create_sale(
            items=items, customer_id=args.customer_id, paid=args.paid
        )
        print(f"Created sale #{sale_id} | Total: {total:.2f} | Paid: {args.paid:.2f}")


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Offline Shop System (SQLite, CLI)"
    )
    parser.add_argument(
        "--db",
        help="Path to SQLite DB file (default: /workspace/offlineshop.db)",
    )
    subparsers = parser.add_subparsers(dest="command", required=True)

    # db commands
    db_parser = subparsers.add_parser("db", help="Database operations")
    db_sub = db_parser.add_subparsers(dest="action", required=True)
    db_init = db_sub.add_parser("init", help="Initialize database schema")
    db_init.set_defaults(func=cmd_db)
    db_backup = db_sub.add_parser("backup", help="Backup database to directory")
    db_backup.add_argument("--out", required=True, help="Output directory")
    db_backup.set_defaults(func=cmd_db)
    db_export = db_sub.add_parser("export-json", help="Export database to JSON file")
    db_export.add_argument("--out", required=True, help="Output JSON file path")
    db_export.set_defaults(func=cmd_db)
    db_import = db_sub.add_parser("import-json", help="Import database from JSON file")
    db_import.add_argument("--file", required=True, help="Input JSON file path")
    db_import.set_defaults(func=cmd_db)

    # product commands
    prod_parser = subparsers.add_parser("product", help="Manage products")
    prod_sub = prod_parser.add_subparsers(dest="action", required=True)
    prod_add = prod_sub.add_parser("add", help="Add a new product")
    prod_add.add_argument("--name", required=True)
    prod_add.add_argument("--sku", required=True)
    prod_add.add_argument("--price", required=True, type=float)
    prod_add.add_argument("--stock", required=True, type=int)
    prod_add.set_defaults(func=cmd_product)

    prod_list = prod_sub.add_parser("list", help="List products")
    prod_list.add_argument("--search")
    prod_list.add_argument("--low", type=int, help="List with stock at or below")
    prod_list.set_defaults(func=cmd_product)

    prod_update = prod_sub.add_parser("update", help="Update product fields")
    prod_update.add_argument("--id", required=True, type=int)
    prod_update.add_argument("--name")
    prod_update.add_argument("--sku")
    prod_update.add_argument("--price", type=float)
    prod_update.set_defaults(func=cmd_product)

    prod_delete = prod_sub.add_parser("delete", help="Delete a product")
    prod_delete.add_argument("--id", required=True, type=int)
    prod_delete.set_defaults(func=cmd_product)

    prod_stock = prod_sub.add_parser("adjust-stock", help="Adjust product stock by delta")
    prod_stock.add_argument("--id", required=True, type=int)
    prod_stock.add_argument("--delta", required=True, type=int)
    prod_stock.set_defaults(func=cmd_product)

    # customer commands
    cust_parser = subparsers.add_parser("customer", help="Manage customers")
    cust_sub = cust_parser.add_subparsers(dest="action", required=True)
    cust_add = cust_sub.add_parser("add", help="Add a new customer")
    cust_add.add_argument("--name", required=True)
    cust_add.add_argument("--phone")
    cust_add.add_argument("--email")
    cust_add.set_defaults(func=cmd_customer)

    cust_list = cust_sub.add_parser("list", help="List customers")
    cust_list.add_argument("--search")
    cust_list.set_defaults(func=cmd_customer)

    # sale commands
    sale_parser = subparsers.add_parser("sale", help="Sales operations")
    sale_sub = sale_parser.add_subparsers(dest="action", required=True)
    sale_create = sale_sub.add_parser("create", help="Create a sale from items")
    sale_create.add_argument(
        "--items",
        required=True,
        help='JSON list like: [{"product_id":1, "qty":2}, ...]',
    )
    sale_create.add_argument("--customer-id", type=int)
    sale_create.add_argument("--paid", required=True, type=float)
    sale_create.set_defaults(func=cmd_sale)

    return parser


def main(argv: list[str] | None = None) -> int:
    parser = build_parser()
    args = parser.parse_args(argv)
    initialize_database(get_db_path(args.db))
    if hasattr(args, "func"):
        args.func(args)
        return 0
    parser.print_help()
    return 1


if __name__ == "__main__":
    raise SystemExit(main())

