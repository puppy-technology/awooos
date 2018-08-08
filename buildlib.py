from fnmatch import filter as fnfilter
from pathlib import Path
import os
import shlex
from subprocess import check_call
import sys

def env(key, default=None):
    return os.environ.get(key, default)


# Miscellaneous file suffix manipulation stuff.

def is_filetype(suffix):
    return lambda x: x.endswith(suffix)


def with_suffix(suffix):
    return lambda x: str(Path(x).with_suffix(suffix))

# ???

def run_command(command, *args):
    command = command(*args)
    text = " ".join(map(shlex.quote, command))
    print("> {}".format(text))
    check_call(command, stdout=sys.stdout)

# Recipes

def recipe_expand(command):
    def cmd(target, match, deps):
        print(command)
        return [x.format(target=target, match=match, deps=deps)
                for x in command]
    return cmd


recipes = {}
def recipe(target, match, deps, command):
    deps = set(deps)
    if isinstance(command, str):
        command = shlex.split(command)
    if isinstance(command, list):
        command = recipe_expand(command)
    recipes[(target, match)] = (deps, command)


def task(target, deps, command):
    recipe(target, None, deps, command)



# Stuff for executing recipes.

def find_recipe(target):
    for (target_pattern, match) in recipes:
        recipe = recipes[(target_pattern, match)]
        deps, command = recipe
        target_parts = target.split(".")
        if (target == target_pattern) or match and \
                (match[0:2] == "%." and match[2:] == target_parts[-1]):
            return (match, deps, command)
    return None

def run_recipe(target):
    if target is None:
        return
    recipe = find_recipe(target)
    if recipe is None:
        print("No rule for target: {}".format(target), file=sys.stderr)
        exit(1)
    match, deps, command = recipe
    run_command(command, *recipe)
    [run_recipe(dep) for dep in deps]


_default = None
def default(name):
    global _default
    _default = name
    return _default


def build(argv=None):
    argv = argv or sys.argv
    if len(argv) > 1:
        args = argv[1:]
    else:
        args = [_default]
    return [run_recipe(arg) for arg in args]
