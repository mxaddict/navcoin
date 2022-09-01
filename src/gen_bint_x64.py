from gen_x86asm import *
import argparse

SUF='_fast'

def gen_add(N, NF=False):
	align(16)
	name = 'mclb_add'
	if NF:
		name += 'NF'
	with FuncProc(f'{name}{N}'):
		if N == 0:
			raise Exception('N = 0')
		with StackFrame(3) as sf:
			z = sf.p[0]
			x = sf.p[1]
			y = sf.p[2]
			for i in range(N):
				mov(rax, ptr(x + 8 * i))
				if i == 0:
					add(rax, ptr(y + 8 * i))
				else:
					adc(rax, ptr(y + 8 * i))
				mov(ptr(z + 8 * i), rax)
			if NF:
				return
			setc(al)
			movzx(eax, al)

def gen_sub(N, NF=False):
	align(16)
	name = 'mclb_sub'
	if NF:
		name += 'NF'
	with FuncProc(f'{name}{N}'):
		if N == 0:
			raise Exception('N = 0')
		with StackFrame(3) as sf:
			z = sf.p[0]
			x = sf.p[1]
			y = sf.p[2]
			for i in range(N):
				mov(rax, ptr(x + 8 * i))
				if i == 0:
					sub(rax, ptr(y + 8 * i))
				else:
					sbb(rax, ptr(y + 8 * i))
				mov(ptr(z + 8 * i), rax)
			setc(al)
			movzx(eax, al)

def gen_mulUnit(N, mode='fast'):
	align(16)
	with FuncProc(f'mclb_mulUnit_{mode}{N}'):
		if N == 0:
			raise Exception('N = 0')
		if N == 1:
			with StackFrame(3) as sf:
				z = sf.p[0]
				x = sf.p[1]
				y = sf.p[2]
				mov(rax, ptr(x))
				mul(y) # [rdx:rax] = x * y
				mov(ptr(z), rax)
				mov(rax, rdx)
				return
		elif N == 2:
			with StackFrame(3, 1, useRDX=True) as sf:
				z = sf.p[0]
				x = sf.p[1]
				y = sf.p[2]
				t = sf.t[0]
				mov(rax, ptr(x))
				mul(y) # [rdx:rax] = x[0] * y
				mov(ptr(z), rax)
				mov(t, rdx)
				mov(rax, ptr(x + 8))
				mul(y) # [rdx:rax] = x[1] * y
				add(rax, t)
				adc(rdx, 0)
				mov(ptr(z + 8), rax)
				mov(rax, rdx)
				return
		else:
			if mode == 'fast':
				with StackFrame(3, 2, useRDX=True) as sf:
					z = sf.p[0]
					x = sf.p[1]
					y = sf.p[2]
					t0 = sf.t[0]
					t1 = sf.t[1]
					mov(rdx, y)
					mulx(t1, rax, ptr(x)) # [y:rax] = x * y
					mov(ptr(z), rax)
					for i in range(1, N-1):
						mulx(t0, rax, ptr(x + i * 8))
						if i == 1:
							add(rax, t1)
						else:
							adc(rax, t1)
						mov(ptr(z + i * 8), rax)
						t0, t1 = t1, t0
					mulx(rax, rdx, ptr(x + (N - 1) * 8))
					adc(rdx, t1)
					mov(ptr(z + (N - 1) * 8), rdx)
					adc(rax, 0)
			else:
				with StackFrame(3, 0, useRDX=True, stackSizeByte=(N - 1) * 2 * 8) as sf:
					z = sf.p[0]
					x = sf.p[1]
					y = sf.p[2]
					posH = (N - 1) * 8
					for i in range(N):
						mov(rax, ptr(x + i * 8))
						mul(y)
						if i == 0: # bypass
							mov(ptr(z), rax)
						else:
							mov(ptr(rsp + (i - 1) * 8), rax)
						if i < N-1:
							mov(ptr(rsp + posH + i * 8), rdx) # don't write the last rdx
					for i in range(N - 1):
						mov(rax, ptr(rsp + posH + i * 8))
						if i == 0:
							add(rax, ptr(rsp + i * 8))
						else:
							adc(rax, ptr(rsp + i * 8))
						mov(ptr(z + (i + 1) * 8), rax)
					adc(rdx, 0)
					mov(rax, rdx)

# [ret:z[N]] = z[N] + x[N] * y
def gen_mulUnitAdd(N, mode='fast'):
	align(16)
	with FuncProc(f'mclb_mulUnitAdd_{mode}{N}'):
		if N == 0:
			raise Exception('N = 0')
		if mode == 'fast':
			with StackFrame(3, 2, useRDX=True) as sf:
				z = sf.p[0]
				x = sf.p[1]
				y = sf.p[2]
				t = sf.t[0]
				L = sf.t[1]
				mov(rdx, y)
				xor_(eax, eax)
				mov(t, ptr(z))
				for i in range(N):
					mulx(rax, L, ptr(x + i * 8))
					adox(t, L)
					mov(ptr(z + i * 8), t)
					if i == N-1:
						break
					mov(t, ptr(z + (i+1) * 8))
					adcx(t, rax)
				mov(t, 0)
				adcx(rax, t)
				adox(rax, t)
		else:
			with StackFrame(3, 0, useRDX=True, stackSizeByte=(N * 2 - 1) * 8) as sf:
				z = sf.p[0]
				x = sf.p[1]
				y = sf.p[2]
				posH = N * 8
				for i in range(N):
					mov(rax, ptr(x + i * 8))
					mul(y)
					mov(ptr(rsp + i * 8), rax)
					if i < N-1:
						mov(ptr(rsp + posH + i * 8), rdx) # don't write the last rdx
				for i in range(N - 1):
					mov(rax, ptr(rsp + (i + 1) * 8))
					if i == 0:
						add(rax, ptr(rsp + posH + i * 8))
					else:
						adc(rax, ptr(rsp + posH + i * 8))
					mov(ptr(rsp + (i + 1) * 8), rax)
				if N > 1:
					adc(rdx, 0)
				for i in range(N):
					mov(rax, ptr(rsp + i * 8))
					if i == 0:
						add(ptr(z + i * 8), rax)
					else:
						adc(ptr(z + i * 8), rax)
				adc(rdx, 0)
				mov(rax, rdx)

def mulPack(pz, offset, py, pd):
	a = rax
	mulx(pd[0], a, ptr(py + 8 * 0))
	mov(ptr(pz + offset), a)
	xor_(a, a)
	n = len(pd)
	for i in range(1, n):
		mulx(pd[i], a, ptr(py + 8 * i))
		adcx(pd[i - 1], a)
	adc(pd[n - 1], 0)

def mulPackAdd(pz, offset, py, hi, pd):
	a = rax
	xor_(a, a)
	n = len(pd)
	for i in range(0, n):
		mulx(hi, a, ptr(py + i * 8))
		adox(pd[i], a)
		if i == 0:
			mov(ptr(pz + offset), pd[0])
		if i == n - 1:
			break
		adcx(pd[i + 1], hi)
	mov(a, 0)
	adox(hi, a)
	adc(hi, a)

def store_mr(m, x):
	n = len(x)
	for i in range(n):
		mov(ptr(m + 8 * i), x[i])

def gen_mulPreN(pz, px, py, pk, t, N):
	mov(rdx, ptr(px + 8 * 0))
	mulPack(pz, 8 * 0, py, pk)
	for i in range(1, N):
		mov(rdx, ptr(px + 8 * i))
		mulPackAdd(pz, 8 * i, py, t, pk)
		s = pk[0]
		pk = pk[1:]
		pk.append(t)
		t = s
	store_mr(pz + 8 * N, pk)

# optimize this later
def gen_mul_fast(N):
	align(16)
	with FuncProc(f'mclb_mul_fast{N}'):
		if N <= 9:
			with StackFrame(3, N+1, useRDX=True) as sf:
				pz = sf.p[0]
				px = sf.p[1]
				py = sf.p[2]
				pk = sf.t[0:N]
				gen_mulPreN(pz, px, py, pk, sf.t[N], N)
		else:
			jmp(f'mclb_mul_slow{N}')

# optimize this later
def gen_sqr_fast(N):
	align(16)
	with FuncProc(f'mclb_sqr_fast{N}'):
		if param.win:
			mov(r8, rdx)
		else:
			mov(rdx, rsi)
		jmp(f'mclb_mul_fast{N}')

"""
def gen_enable_fast(N):
	align(16)
	with FuncProc('mclb_disable_fast'):
		for i in range(1, N):
			lea(rdx, rip(f'mclb_mulUnit{i}'))
			lea(rax, rip(f'mclb_mulUnit_slow{i}'))
			mov(ptr(rdx), rax)
		for i in range(1, N):
			lea(rdx, rip(f'mclb_mulUnitAdd{i}'))
			lea(rax, rip(f'mclb_mulUnitAdd_slow{i}'))
			mov(ptr(rdx), rax)
		ret()
"""

def gen_udiv128():
	align(16)
	with FuncProc('mclb_udiv128'):
		mov(rax, rdx);
		mov(rdx, rcx);
		div(r8);
		mov(ptr(r9), rdx);
		ret();


parser = argparse.ArgumentParser()
parser.add_argument('-win', '--win', help='output win64 abi', action='store_true')
parser.add_argument('-n', '--num', help='max size of Unit', type=int, default=9)
parser.add_argument('-addn', '--addn', help='max size of add/sub', type=int, default=16)
parser.add_argument('-gas', '--gas', help='output gas syntax', default=False, action='store_true')
parser.add_argument('-m', '--mode', help='output asm syntax', default='nasm')
param = parser.parse_args()

setWin64ABI(param.win)
N = param.num
addN = param.addn

init(param.mode)
"""
segment('data')
defineName('mclb_mulUnitTbl')
for i in range(N+1):
	defineName(f'mclb_mulUnit{i}')
	if i == 0:
		dq_(0)
	else:
		dq_(f'mclb_mulUnit_fast{i}')

defineName('mclb_mulUnitAddTbl')
for i in range(N+1):
	defineName(f'mclb_mulUnitAdd{i}')
	if i == 0:
		dq_(0)
	else:
		dq_(f'mclb_mulUnitAdd_fast{i}')
"""
segment('text')

for i in range(1,addN+1):
	gen_add(i)

for i in range(1,addN+1):
	gen_sub(i)

for i in range(1,addN+1):
	gen_add(i, True)

for i in range(1,addN+1):
	gen_sub(i, True)

for i in range(1,N+1):
	gen_mulUnit(i, 'fast')

for i in range(1,N+1):
	gen_mulUnitAdd(i, 'fast')

for i in range(1,N+1):
	gen_mulUnit(i, 'slow')

for i in range(1,N+1):
	gen_mulUnitAdd(i, 'slow')

for i in range(1,N+1):
	gen_mul_fast(i)

for i in range(1,N+1):
	gen_sqr_fast(i)

#gen_enable_fast(N)

if param.win:
	gen_udiv128()

term()
