all:
	sed 's/^# /\\newpage\n# /g' techreport.md  > techreport2.md
	pandoc --metadata-file=custom.yaml --toc -V documentclass=report  --from=markdown+multiline_tables -H custom.tex techreport2.md -o techreport.pdf
	rm techreport2.md
	mv techreport.pdf ..
