
import os
import random
import string
from pathlib import Path

def generate_test_files(output_dir="test/test_files", num_files=400):
    """Genera archivos de prueba con contenido mixto"""
    try:
        # Crear directorio si no existe
        Path(output_dir).mkdir(parents=True, exist_ok=True)
        
        # Tamaños entre 1KB y 5MB
        min_size = 1024  # 1KB
        max_size = 5 * 1024 * 1024  # 5MB
        
        # Patrones para contenido repetitivo (mejor compresión)
        patterns = [
            "Lorem ipsum dolor sit amet ",
            "1234567890ABCDEF ",
            "The quick brown fox jumps ",
            "0101010101010101 ",
            "REPETITIVE_CONTENT_"
        ]
        
        print(f"Generando {num_files} archivos en {output_dir}...")
        
        for i in range(1, num_files + 1):
            filename = os.path.join(output_dir, f"test_{i:04d}.txt")
            file_size = random.randint(min_size, max_size)
            
            # 30-70% de contenido repetitivo
            repetitive_ratio = random.uniform(0.3, 0.7)
            repetitive_size = int(file_size * repetitive_ratio)
            random_size = file_size - repetitive_size
            
            # Generar contenido
            with open(filename, 'w') as f:
                # Parte repetitiva
                pattern = random.choice(patterns)
                repetitions = (repetitive_size // len(pattern)) + 1
                f.write((pattern * repetitions)[:repetitive_size])
                
                # Parte aleatoria
                f.write(''.join(random.choices(
                    string.ascii_letters + string.digits + string.punctuation + ' \n\t',
                    k=random_size
                )))
            
            if i % 100 == 0:
                print(f"Generados {i}/{num_files} archivos...")
        
        print("¡Generación completada exitosamente!")
        return True
        
    except Exception as e:
        print(f"\nERROR: No se pudieron generar archivos - {str(e)}")
        return False

if __name__ == "__main__":
    generate_test_files()