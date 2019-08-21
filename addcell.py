import golly as g 

cells = g.getcells(g.getrect())

g.show(str(cells))
for i in range(0, len(cells), 3):
	
	x = cells[i]
	y = cells[i + 1]
	
	if g.getcell(x, y) != 1:
		continue
		
	for dx in range(-1, 2):
		for dy in range(-1, 2):
			if g.getcell(x + dx, y + dy) == 0:
				g.setcell(x + dx, y + dy, 2)
				