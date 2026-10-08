# BetterPause-Geode - Claude Instructions

## graphify
This project has a knowledge graph at graphify-out/ with god nodes, community structure, and cross-file relationships.

Rules:
- For codebase questions or feature planning, first check `graphify-out/graph.json` or run `graphify query "<question>"`.
- Use `graphify path "<A>" "<B>"` for relationships and `graphify explain "<concept>"` for focused concepts.
- When creating a feature, refactoring, or fixing bugs:
  1. Map the blast radius using the graph to find all dependent components before editing.
  2. Use graph-provided file paths (`source_file`) rather than blind grepping.
  3. **Auto-update rule:** Always run `graphify update .` at the end of the task whenever files were modified or created (run once at the end of the batch, not between intermediate edits).
