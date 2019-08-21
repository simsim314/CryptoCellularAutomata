import golly as g 

def add(d, idx):
	new_str = "" 
	
	
	for i in range(10):
		for j in range(10):
			if g.getcell(i + d, j) == 1:
				dx = i 
				dy = j 
				break
				
			
	for i in range(10):
		for j in range(10):
			if g.getcell(i + d, j) == 1:
				new_str += "locators[%d].locations.push_back(locatorbit(%d,%d,false));\n" % (idx, i - dx, j - dy)
	
	for i in range(10):
		for j in range(10):
			if g.getcell(i + d, j) == 2:
				new_str += "locators[%d].locations.push_back(locatorbit(%d,%d,true));\n" % (idx, i - dx, j - dy)
	
	return new_str

d = 0 
full = ""

while True: 	
	if len(g.getcells([d, 0, 10, 10])) == 0:
		g.setclipstr(full)
		g.exit()
		
	full += "locators.push_back(locator());\n"
	full += add(d, d / 10)
	d += 10
	
		
	