'''
import urllib.request

print('Beginning file download with urllib2...')

for i in range(4,70):
	try:
		url = 'http://catagolue.appspot.com/textcensus/b3s23/C1/xs' + str(i)
		urllib.request.urlretrieve(url, 'census/xs' + str(i) + '.txt')
	except: 
		pass'''
		
import glob
files = (glob.glob("census/*.txt"))
import pyperclip

res = ""

for fname in files:
	f = open(fname, "r")
	lines = f.readlines()
	
	for l in lines:
		data = l.strip('\n').split(',')
		
		if len(data) == 2 and data[0].strip("\"") != "apgcode":
		
			res += data[0].strip("\"") + ","+ str(int(20000000000000 / int(data[1].strip("\"")))) + "\n"
	
	f.close()
	
pyperclip.copy(res)