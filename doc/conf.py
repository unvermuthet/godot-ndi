# -*- coding: utf-8 -*-
#
# Godot Engine documentation build configuration file

import sphinx
import sphinx_rtd_theme
import sys
import os

# -- General configuration ------------------------------------------------

needs_sphinx = "8.1"

# Sphinx extension module names and templates location
sys.path.append(os.path.abspath("godot-docs/_extensions"))
extensions = [
    "sphinx_tabs.tabs",
    "notfound.extension",
    "sphinxext.opengraph",
    "sphinx_copybutton",
    "sphinxcontrib.video",
    "myst_parser",
]

# Warning when the Sphinx Tabs extension is used with unknown
# builders (like the dummy builder) - as it doesn't cause errors,
# we can ignore this so we still can treat other warnings as errors.
sphinx_tabs_nowarn = True

# Disable collapsing tabs for codeblocks.
sphinx_tabs_disable_tab_closing = True

# Custom 4O4 page HTML template.
# https://github.com/readthedocs/sphinx-notfound-page
notfound_context = {
    "title": "Page not found",
    "body": """
        <h1>Page not found</h1>
        <p>
            Sorry, we couldn't find that page. It may have been renamed or removed
            in the version of the documentation you're currently browsing.
        </p>
        <p>
            If you're currently browsing the
            <em>latest</em> version of the documentation, try browsing the
            <a href="/en/stable/"><em>stable</em> version of the documentation</a>.
        </p>
        <p>
            Alternatively, use the
            <a href="#" onclick="$('#rtd-search-form [name=\\'q\\']').focus()">Search docs</a>
            box on the left or <a href="/">go to the homepage</a>.
        </p>
    """,
}

# on_rtd is whether we are on readthedocs.org, this line of code grabbed from docs.readthedocs.org
on_rtd = os.environ.get("READTHEDOCS", None) == "True"

# Don't add `/en/latest` prefix during local development.
# This makes it easier to test the custom 404 page by loading `/404.html`
# on a local web server.
if not on_rtd:
    notfound_urls_prefix = ''

# Specify the site name for the Open Graph extension.
ogp_site_name = "Godot NDI documentation"
ogp_social_cards = {
    "enable": False
}

if not os.getenv("SPHINX_NO_GDSCRIPT"):
    extensions.append("gdscript")

if not os.getenv("SPHINX_NO_DESCRIPTIONS"):
    extensions.append("godot_descriptions")

templates_path = ["_templates", "godot-docs/_templates"]

# You can specify multiple suffix as a list of string: ['.rst', '.md']
source_suffix = {'.rst': 'restructuredtext', '.md': 'markdown'}
source_encoding = "utf-8-sig"

# The master toctree document
master_doc = "index"

# General information about the project
project = "Godot NDI"
author = "Henry Muth - unvermuthet and Godot NDI contributors"
copyright = (
    f"2025-present {author} (CC BY 4.0)"
)

# Version info for the project, acts as replacement for |version| and |release|
# The short X.Y version
version = os.getenv("READTHEDOCS_VERSION", "latest")
# The full version, including alpha/beta/rc tags
release = version

# Parse Sphinx tags passed from RTD via environment
env_tags = os.getenv("SPHINX_TAGS")
if env_tags is not None:
    for tag in env_tags.split(","):
        print("Adding Sphinx tag: %s" % tag.strip())
        tags.add(tag.strip())  # noqa: F821

# Language / i18n

supported_languages = {
    "en": "Godot NDI %s documentation in English"
}

# RTD normalized their language codes to ll-cc (e.g. zh-cn),
# but Sphinx did not and still uses ll_CC (e.g. zh_CN).
# `language` is the Sphinx configuration so it needs to be converted back.
language = os.getenv("READTHEDOCS_LANGUAGE", "en")
if "-" in language:
    (lang_name, lang_country) = language.split("-")
    language = lang_name + "_" + lang_country.upper()

if not language in supported_languages.keys():
    print("Unknown language: " + language)
    print("Supported languages: " + ", ".join(supported_languages.keys()))
    print(
        "The configured language is either wrong, or it should be added to supported_languages in conf.py. Falling back to 'en'."
    )
    language = "en"

is_i18n = tags.has("i18n")  # noqa: F821
print("Build language: {}, i18n tag: {}".format(language, is_i18n))

exclude_patterns = [".*", "**/.*", "_build", "_tools", "godot-docs/**"]

# fmt: off
# These imports should *not* be moved to the start of the file,
# they depend on the sys.path.append call registering "_extensions".
# GDScript syntax highlighting
from gdscript import GDScriptLexer
from sphinx.highlighting import lexers

lexers["gdscript"] = GDScriptLexer()
# fmt: on

smartquotes = False

# Pygments (syntax highlighting) style to use
pygments_style = "sphinx"
highlight_language = "gdscript"

# -- Options for HTML output ----------------------------------------------

html_theme = "sphinx_rtd_theme"
if on_rtd:
    using_rtd_theme = True

# Theme options
html_theme_options = {
    # if we have a html_logo below, this shows /only/ the logo with no title text
    "logo_only": True,
    # Collapse navigation (False makes it tree-like)
    "collapse_navigation": False,
    # Remove version and language picker beneath the title
    "version_selector": False,
    "language_selector": False,
    # Set Flyout menu to attached
    "flyout_display": "attached",
}

html_title = supported_languages[language] % ("(" + version + ")")

# Edit on GitHub options: https://docs.readthedocs.io/en/latest/guides/edit-source-links-sphinx.html
html_context = {
    "display_github": not is_i18n,  # Integrate GitHub
    "github_user": "unvermuthet",  # Username
    "github_repo": "godot-ndi",  # Repo name
    "github_version": "main",  # Version
    "conf_py_path": "/doc/",  # Path in the checkout to the docs root
    "godot_docs_title": supported_languages[language],
    "godot_docs_basepath": "https://godot-ndi.readthedocs.io/",
    "godot_docs_suffix": ".html",
    # Distinguish local development website from production website.
    # This prevents people from looking for changes on the production website after making local changes :)
    "godot_title_prefix": "" if on_rtd else "(DEV) ",
    # Set this to `True` when in the `latest` branch to clearly indicate to the reader
    # that they are not reading the `stable` documentation.
    "godot_is_latest": version == "latest",
    "godot_version": version,
    # Enables a banner that displays the up-to-date status of each article.
    "godot_show_article_status": False,
    # Display user-contributed notes at the bottom of pages that don't have `:allow_comments: False` at the top.
    # "godot_show_article_comments": on_rtd and not is_i18n,
    "godot_show_article_comments": True,
}

html_logo = "../project/addons/godot-ndi/icons/VideoStreamNDI.svg"

# These folders are copied to the documentation's HTML output
html_static_path = ["godot-docs/_static", "_static"]

html_extra_path = ["robots.txt"]

# These paths are either relative to html_static_path
# or fully qualified paths (e.g. https://...)
html_css_files = [
    "css/custom.css",
]

if not on_rtd:
    html_css_files.append("css/dev.css")

html_js_files = [
    "js/custom.js"
]

# Output file base name for HTML help builder
htmlhelp_basename = "GodotNDIdoc"

# -- Options for reStructuredText parser ----------------------------------

# Enable directives that insert the contents of external files
file_insertion_enabled = False

# -- Options for LaTeX output ---------------------------------------------

# Grouping the document tree into LaTeX files. List of tuples
# (source start file, target name, title,
#  author, documentclass [howto, manual, or own class]).
latex_documents = [
    (
        master_doc,
        "GodotNDI.tex",
        "Godot NDI Documentation",
        author,
        "manual",
    ),
]

# -- Options for linkcheck builder ----------------------------------------

# disable checking urls with about.html#this_part_of_page anchors
linkcheck_anchors = False

linkcheck_timeout = 10

# Needed so the table of contents is created for EPUB
epub_tocscope = 'includehidden'
