all:
	halibut --pdf=radare.pdf radare.but
	halibut --text=radare.pdf.txt radare.but > /dev/null 2>&1
	halibut --html=radare.pdf.html radare.but
	mkdir -p html && cd html && halibut --html ../radare.but

push:
	scp radare.pdf radare.pdf.html radare.org:/srv/http/radareorg/get
	rsync -avz html/* radare.org:/srv/http//radareorg/doc/html
