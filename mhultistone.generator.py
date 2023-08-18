import os
from jinja2 import Environment, FileSystemLoader

ncores = os.cpu_count()
ngen = 2 * ncores
env = Environment(loader=FileSystemLoader("templates/"))

fnames = ["mhultistone.c", "mhultistone.h"]
templates = {f:env.get_template(f + ".jinja") for f in fnames}

for fname,template in templates.items():
    with open("src/" + fname, "w") as fout:
        fout.write(template.render(n = ngen, ncores=ncores))
