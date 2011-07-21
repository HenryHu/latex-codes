@echo off

latex p
bibtex p
latex p
latex p
dvips p
ps2pdf p.ps


