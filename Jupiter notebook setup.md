# SET UP Jupyter Notebook Project

## 1. Create Project Folder
Create a new folder for your project.

## 2. Navigate to the Project Folder
Open a terminal or PowerShell and navigate to your project folder:
```powershell
cd path\to\project_folder
```

## 3. Create a Virtual Environment
Run the following command to create a virtual environment:
```powershell
python -m venv name_venv
```

## 4. Activate the Virtual Environment
Activate the virtual environment with:
```powershell
./name_venv/Scripts/Activate
```

## 5. Install Jupyter
Once the virtual environment is activated, install Jupyter:
```powershell
pip install jupyter
```

## 6. Install ipykernel
Install the `ipykernel` package to allow Jupyter to recognize the virtual environment as a kernel:
```powershell
pip install ipykernel
```

## 7. Add Virtual Environment to Jupyter
Register the virtual environment as a Jupyter kernel:
```powershell
python -m ipykernel install --user --name=name_venv --display-name "Python (name_venv)"
```

## 8. Launch Jupyter Notebook
Start Jupyter Notebook with:
```powershell
jupyter notebook
```

Your Jupyter Notebook is now set up and ready to use! 🚀

### Note that you have to enable Scripts execution in you system ,if you have not done t yet juste run this command in powershell
Set-ExecutionPolicy RemoteSigned -Scope CurrentUser

