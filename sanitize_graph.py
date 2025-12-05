import random

def add_weights(input_file, output_file):
  with open(input_file, "r") as file, open(output_file, "w") as out:
    for line in file:
      # Keep header as-is
      if line.startswith("#") or not line.strip():
        out.write(line)
        continue

      parts = line.split()
      if len(parts) != 2:
        continue

      from_node, to_node = parts
      weight = random.randint(0, 100)

      # Write: FromNodeId[TAB]ToNodeId"   "Weight
      out.write(f'{from_node}\t{to_node}\t{weight}\n')


# Example usage:
add_weights("com-dblp.ungraph.txt", "graph_with_weights.txt")