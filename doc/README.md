### Building the docs locally

- Install python requirements: `python3 -m pip install -r requirements.txt`
- If API changes were made:
	- Update class `.xml` files with `godot --path project --doctool ../ --gdextension-docs`
- Run `./doc/build.sh`
- The VS Code extension `Live Server` is great for viewing
