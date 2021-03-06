# move to rapy api
try:
	import r
except:
	pass

class Program():
	def update(self):
		self.arch = r.cmd("e asm.arch")
		self.bits = eval(r.cmd("e asm.bits"))
		self.os = r.cmd("e asm.os")
		self.type = r.cmd("e file.type")
		self.size = eval(r.cmd("i~size[1]#1"))
		self.vaddr = eval(r.cmd("e io.vaddr"))
		self.paddr = eval(r.cmd("e io.paddr"))
		self.bigendian = r.cmd("e cfg.bigendian")
		self.entrypoint = eval(r.cmd("?v entrypoint"))

	def __init__(self):
		self.update()

# functions
class Function():
	def __init__(self):
		self.name = 0
		self.addr = 0
		self.size = 0

	def analyze(addr):
		r.cmd(".af*@%s"%addr)
	analyze = staticmethod(analyze)

	def add(name, addr, size):
		r.cmd("CF %s@%s"%(size,addr))
		r.cmd("f %s@%s"%(name,addr))
	add = staticmethod(add)

	def remove(addr):
		r.cmd("CF-@%s"%addr)
		r.cmd("f -%s"%addr)
	remove = staticmethod(remove)

class Functions():
	def update(self):
		self.list = []
		items = r.cmd("CF").split('\n')
		for a in items:
			words = a.split(' ')
			fun = Function()
			fun.size = eval(words[1])
			fun.addr = eval(words[3])
			fun.name = words[5]
			self.list.append(fun)

	def __init__(self):
		self.update()

# comments

class Comment():
	def __init__(self):
		self.addr = 0
		self.comment = ''

	def add(addr, str):
		r.cmd("CC %s@%s"%(str, addr))
	add = staticmethod(add)

	def remove(addr, str):
		r.cmd("CC -%s@%s"%(str, addr))
	remove = staticmethod(remove)

class Comments():
	def update():
		self.list = []
		for a in r.cmd("CC").split('\n'):
			words = a.split('@')
			c = Comment()
			c.addr = eval(words[1])
			c.comment = words[0][3:]
			self.list.append(cmt)
	def __init__(self):
		self.update()

class Xref():
	def __init__(self):
		self.addr = 0
		self.endaddr = 0

class CodeXrefs():
	def update(self, addr):
		self.list = []
		for a in r.cmd("Cx").split("\n"):
			words = a.split(' ')
			a_addr = eval(words[1])
			a_endaddr = eval(words[3])
			if addr == None or addr == a_endaddr:
				x = Xref()
				x.addr = a_addr
				x.endaddr = a_endaddr
				self.list.append(x)
	def __init__(self, addr=None):
		self.update(addr)

class DataXrefs():
	def update(self, addr):
		self.list = []
		for a in r.cmd("CX").split("\n"):
			words = a.split(' ')
			if len(a) < 3:
				continue
			a_addr = eval(words[1])
			a_endaddr = eval(words[3])
			if addr == None or addr == a_endaddr:
				x = Xref()
				x.addr = a_addr
				x.endaddr = a_endaddr
				self.list.append(x)
	def __init__(self, addr=None):
		self.update(addr)

# imports

class Import():
	def __init__(self):
		self.name = ''
		self.addr = 0
		self.endaddr = 0
		self.size = 0

class Imports():
	# TODO: Use !rabin instead
	def update(self):
		self.list = []
		items = r.cmd("f~imp.").split('\n')
		for a in items:
			# addr size name
			words = a.split(' ')
			sec = Symbol()
			sec.name = words[2].replace('imp.','')
			sec.addr = eval(words[0])
			sec.size = eval(words[1])
			sec.endaddr = sec.addr + sec.size
			self.list.append(sec)

	def __init__(self):
		self.update()

class Symbol():
	def __init__(self):
		self.name = ''
		self.addr = 0
		self.endaddr = 0
		self.size = 0

class Symbols():
	# TODO: Use !rabin instead
	def update(self):
		self.list = []
		items = r.cmd("f~sym.").split('\n')
		for a in items:
			# addr size name
			words = a.split(' ')
			sec = Symbol()
			sec.name = words[2].replace('sym.','')
			sec.addr = eval(words[0])
			sec.size = eval(words[1])
			sec.endaddr = sec.addr + sec.size
			self.list.append(sec)

	def __init__(self):
		self.update()
		
# sections
class Section():
	def __init(self):
		self.name = ''
		self.addr = 0
		self.endaddr = 0
		self.size = 0

class Sections():
	# TODO: Use !rabin instead
	def update(self):
		self.list = []
		items = r.cmd("f~section.").split('\n')
		sec = Section()
		for a in items:
			words = a.split(' ')
			if a.find('_end') == -1:
				sec = Section()
				sec.addr = eval(words[0])
				sec.name = words[2].replace('section.','')
			else:
				sec.endaddr = eval(words[0])
				sec.size = sec.endaddr - sec.addr
				if sec.name != '':
					self.list.append(sec)

	def __init__(self):
		self.update()

# basic blocks
class BasicBlock():
	def __init__(self):
		self.addr = 0
		self.type = ''
		self.size = 0
		self.bytes = '' 
		self.j_true = 0
		self.j_false = 0
		self.calls = []

class BasicBlocks():
	def update(self, addr):
		self.list = []
		bb = BasicBlock()
		for line in r.cmd("ab 128 @ %s"%addr).split('\n'):
			words = line.split('=')
			words[0] = words[0][:-1] # strip ' '
			if words[0] == 'offset':
				bb = BasicBlock()
				bb.addr = eval(words[1])
			elif words[0] == 'type':
				bb.type = words[1]
			elif words[0] == 'size':
				bb.size  = eval(words[1])
			elif words[0] == 'true':
				bb.j_true = eval(words[1])
			elif words[0] == 'false':
				bb.j_false = eval(words[1])
			elif words[0] == 'bytes':
				bb.bytes = words[1]
				self.list.append(bb)
			elif words[0][:4] == 'call':
				bb.calls.append(eval(words[1]))

	def __init__(self, addr):
		self.update(addr);

# opcodes

class Opcode():
	def assemble(addr, op):
		r.cmd("wa %s@%d"%(op,addr))
	assemble = staticmethod(assemble)

	def disassemble(addr, n):
		return r.cmd("pd %s@%d"%(n,addr))
	disassemble = staticmethod(disassemble)

	def __init__(self):
		self.opcode = ''
		self.addr = 0
		self.size = 0
		self.stackop = ''
		self.bytes = ''
		self.type = ''
		self.ref = 0
		self.j_true = 0
		self.j_false = 0

	def __init__(self, addr):
		for line in r.cmd("ao@%s"%addr):
			words = line.split('=')
			words[0] = words[0][:-1] # strip ' '
			if words[0] == 'opcode':
				self.opcode = words[1]
			elif words[0] == 'size':
				self.size = eval(words[1])
			elif words[0] == 'stackop':
				self.stackop = words[1]
			elif words[0] == 'type':
				self.type = words[1]
			elif words[0] == 'bytes':
				self.bytes = words[1]
			elif words[0] == 'offset':
				self.addr = words[1]
			elif words[0] == 'ref':
				self.ref = eval(words[1])
			elif words[0] == 'jump':
				self.j_true = eval(words[1])
			elif words[0] == 'fail':
				self.j_false = eval(words[1])

class Opcodes():
	def update(self, addr, endaddr):
		self.list = []
		while addr < endaddr:
			op = Opcode(addr)
			self.list.append(op)
			addr = addr + op.size

	def __init__(self, addr, endaddr):
		self.update(addr, endaddr)

# graph
class Graph():
	def make_dot(addr, file):
		r.cmd("s %s"%addr)
		r.cmd("agd %s"%file)
	make_dot = staticmethod(make_dot)

	def make_png(addr, file):
		r.cmd("s %s"%addr)
		r.cmd("agd %s.dot"%file)
		r.cmd("!!dot -Tpng -o %s %s.dot"%(file,file));
		r.cmd("!!rm %s.dot"%file);
	make_png = staticmethod(make_png)

	def add_node(addr, size, cmd):
		r.cmd("gun %s %s %s"%(addr, size, cmd))

	def add_edge(addr, endaddr):
		r.cmd("gue %s %s"%(addr, endaddr))

	def reset():
		r.cmd("gur")

	def view():
		r.cmd("guv")

	def dot(file):
		r.cmd("gud > %s"%file)
	add_node = staticmethod(add_node)
	add_edge = staticmethod(add_edge)
	reset = staticmethod(reset)
	view = staticmethod(view)
	
# vm
class VM():
	def __init__(self):
		print "VM : TODO"

def idc_import(file):
	r.cmd(".!rsc idc2rdb %s"%file)
