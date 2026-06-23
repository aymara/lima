# Algorithm
  - Check tokens dictionary.
    - If present, use it
  - Check PoS dictionary.
    - If present, use it.
    - If PoS present, don't try model, use lower raw token, except for PROPN, use raw token as is
  - Use lemmatizer model
