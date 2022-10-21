from dataclasses import dataclass
from enum import Enum
import configparser, glob, re, sys

def error(msg: str):
  if sys.stderr.isatty():
    sys.stderr.write("\x1b[31m")
  
  sys.stderr.write("error: " + str(msg))

  if sys.stderr.isatty():
    sys.stderr.write("\x1b[m")
  
  sys.stderr.write("\n")

class CmdType(Enum):
  ONE_TO_ONE = 0
  MANY_TO_ONE = 1
  ONE_TO_MANY = 2

CMD_TYPE_STRING = {
  "121": CmdType.ONE_TO_ONE,
  "one to one": CmdType.ONE_TO_ONE,
  "one2one": CmdType.ONE_TO_ONE,
  "one-one": CmdType.ONE_TO_ONE,
  "one->one": CmdType.ONE_TO_ONE,
  "m21": CmdType.MANY_TO_ONE,
  "many to one": CmdType.MANY_TO_ONE,
  "many2one": CmdType.MANY_TO_ONE,
  "many-one": CmdType.MANY_TO_ONE,
  "many->one": CmdType.MANY_TO_ONE,
  "12m": CmdType.ONE_TO_MANY,
  "one to many": CmdType.ONE_TO_MANY,
  "one2many": CmdType.ONE_TO_MANY,
  "one-many": CmdType.ONE_TO_MANY,
  "one->many": CmdType.ONE_TO_MANY,
}

@dataclass
class CmdCfg:
  typ: CmdType
  cmd: str
  inp: str
  out: str

@dataclass
class ProjectCfg:
  name: str
  desc: str
  srcs: list[str]
  cmds: list[str]

class CfgFile:
  def translate(self) -> list[str]:
    raise NotImplementedError

@dataclass
class CommandsCfgFile(CfgFile):
  cmds: dict[str, CmdCfg]

  def translate(self) -> list[str]:
    l: list[str] = []
    for name, cfg in self.cmds.items():
      l.append(f"rule {name}")
      l.append(f"  command = {cfg.cmd}")
    return l

@dataclass
class ProjectCfgFile(CfgFile):
  cfg: ProjectCfg
  vars: dict[str, str]
  includes: dict[str, CfgFile]

  def translate(self) -> list[str]:
    l: list[str] = []

    cmds: dict[str, CmdCfg] = {}

    for path, file in self.includes.items():
      l.append(f"include {path}.ninja")
      if isinstance(file, CommandsCfgFile):
        for k, v in file.cmds.items():
          cmds[k] = v
    
    l.append("")

    for name, value in self.vars.items():
      l.append(f"{name} = {value}")
    
    l.append("")

    src = [file for path in self.cfg.srcs for file in glob.glob(path)]

    for name in self.cfg.cmds:
      inp_pat = re.compile(cmds[name].inp)
      inp_src = []
      for file in src:
        if inp_pat.fullmatch(file): inp_src.append(file)
      if cmds[name].typ == CmdType.ONE_TO_ONE:
        out_src = [inp_pat.sub(cmds[name].out, file) for file in inp_src]
        for inp_file, out_file in zip(inp_src, out_src):
          l.append(f"build {out_file}: {name} {inp_file}")
          src.append(out_file)
      elif cmds[name].typ == CmdType.MANY_TO_ONE:
        l.append(f"build {cmds[name].out}: {name} {' '.join(inp_src)}")
      elif cmds[name].typ == CmdType.ONE_TO_MANY:
        error("the 'one to many' command type is not implemented yet!")
    
    return l

def parse_command_cfg(parser: configparser.ConfigParser, section: str) -> CmdCfg:
  return CmdCfg(
    typ = CMD_TYPE_STRING[parser.get(section, "typ")],
    inp = parser.get(section, "inp"),
    out = parser.get(section, "out"),
    cmd = parser.get(section, "cmd"))

def parse_commands_cfg(parser: configparser.ConfigParser) -> CommandsCfgFile:
  return CommandsCfgFile({ section: parse_command_cfg(parser, section) for section in parser.sections() })

def parse_project_cfg(parser: configparser.ConfigParser) -> ProjectCfgFile:
  cfg = ProjectCfgFile(
    cfg = ProjectCfg(
      name = parser.get("cfg", "name"),
      desc = parser.get("cfg", "desc"),
      cmds = parser.get("cfg", "cmds").split(),
      srcs = parser.get("cfg", "srcs").split(),
    ),
    vars = dict(parser.items("var")),
    includes = {}
  )

  for include in parser.get("DEFAULT", "includes").split():
    cfg.includes[include] = parse_cfg(include)
  
  return cfg

def parse_cfg(file: str) -> CfgFile:
  parser = configparser.ConfigParser()
  with open(file) as f:
    parser.read_string("[DEFAULT]\n" + f.read(), file)
  filetype = parser.get("DEFAULT", "type")
  if filetype == "commands":
    return parse_commands_cfg(parser)
  elif filetype == "project":
    return parse_project_cfg(parser)
  
cfg = parse_cfg("dcore/project.cfg")
print(*cfg.translate(), sep='\n')
