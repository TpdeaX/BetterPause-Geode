# Reglas del Proyecto - BetterPause-Geode

## Flujo de Trabajo con Grafo de Conocimiento (Graphify)
Este proyecto cuenta con un grafo de conocimiento persistente en `graphify-out/graph.json` con mapa de dependencias, nodos clave y comunidades.

### 1. Al crear una Feature o Modificaci?n (Refactor / Bugfix)
1. **Mapeo de Impacto (Blast Radius):** Consulta el grafo para ubicar los nodos afectados y sus vecinos antes de modificar c?digo.
2. **Localizaci?n Directa:** Usa las rutas (`source_file`) provistas por el grafo para evitar b?squedas a ciegas.
3. **Actualizaci?n Autom?tica al Finalizar Tarea:**
   - Si durante la sesi?n se modific? o cre? c?digo (cualquier archivo relevante), **ejecuta `graphify update .` al terminar el lote de cambios**, antes de dar por finalizada la respuesta.
   - *Nota de rendimiento:* No ejecutes el update entre cambios intermedios (ej. si vas a editar 3 archivos consecutivos, haz los 3 cambios y corre el update una sola vez al final).

### 2. Al responder preguntas o investigar errores
- Consulta primero `graphify-out/graph.json`, `graphify query "<pregunta>"` o las herramientas MCP de Graphify antes de leer archivos sueltos.
