import golly as g 

res = ""

for i in range(256):
	res += "Iter" + str(i) + ":" + str(g.getcells(g.getrect())).strip("]").strip("[") + "\n"
	g.run(1)
	
g.setclipstr(res)