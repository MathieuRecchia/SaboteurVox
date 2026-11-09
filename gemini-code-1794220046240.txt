# 1. Renommer CMakeLists.txt.txt en CMakeLists.txt
if (Test-Path "CMakeLists.txt.txt") {
    Rename-Item -Path "CMakeLists.txt.txt" -NewName "CMakeLists.txt" -Force
}

# 2. Enlever d'éventuelles doubles extensions .txt dans le dossier Source/
Get-ChildItem -Path "Source/*.txt" -ErrorAction SilentlyContinue | Rename-Item -NewName { $_.Name -replace '\.txt$','' }

# 3. Supprimer le dossier cache .vs et réinitialiser complètement Git
Remove-Item -Path ".github/workflows/.vs" -Recurse -Force -ErrorAction SilentlyContinue
Remove-Item -Path "Source/.git" -Recurse -Force -ErrorAction SilentlyContinue
Remove-Item -Path ".git" -Recurse -Force -ErrorAction SilentlyContinue

# 4. Vérifier que les 4 fichiers C++ sont bien présents dans Source/
Get-ChildItem -Path "Source"

# 5. Créer un commit propre avec les 6 fichiers du projet et pousser vers GitHub
git init
git add -A
git commit -m "Structure complete Saboteur Vox (CMake + Workflow + 6 onglets)"
git branch -M main
git remote add origin https://github.com/MathieuRecchia/SaboteurVox.git
git push -u origin main --force