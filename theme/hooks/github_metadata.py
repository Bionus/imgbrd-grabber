from typing import Any, Dict, Optional

import os
from contextvars import ContextVar

import requests


context_latest_release = ContextVar("github_latest_release", default=None)


def load_github_metadata() -> None:
    """Load GitHub metadata from the API."""

    # Only load the metadata once
    if context_latest_release.get():
        return

    owner = "Bionus"
    repo = "imgbrd-grabber"
    token = os.environ["GITHUB_TOKEN"]

    headers = {
        "Accept": "application/vnd.github+json",
        "Authorization": f"Bearer {token}",
        "X-GitHub-Api-Version": "2022-11-28",
    }
    response = requests.get(f"https://api.github.com/repos/{owner}/{repo}/releases/latest", headers=headers)

    context_latest_release.set(response.json())


def define_env(env) -> None:
    """Gives access to site.github inside Markdown pages using mkdocs-macros-plugin."""

    # Ensure the GitHub metadata is loaded
    load_github_metadata()

    # Create the "site.github" container
    if "site" not in env.variables:
        env.variables["site"] = {}
    if "github" not in env.variables["site"]:
        env.variables["site"]["github"] = {}

    # Set various variables
    env.variables["site"]["github"]["latest_release"] = context_latest_release.get()


def on_page_context(context: Dict[str, Any], **kwargs) -> Optional[Dict[str, Any]]:
    """Gives access to site.github inside HTML pages using MkDocs hooks."""

    # Ensure the GitHub metadata is loaded
    load_github_metadata()

    # Create the "site.github" container
    if "site" not in context:
        context["site"] = {}
    if "github" not in context["site"]:
        context["site"]["github"] = {}

    # Set various variables
    context["site"]["github"]["latest_release"] = context_latest_release.get()

    return context
