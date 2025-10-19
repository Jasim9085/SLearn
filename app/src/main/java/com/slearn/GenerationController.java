package com.slearn;

import android.content.Context;
import org.json.JSONException;
import org.json.JSONObject;
import java.io.IOException;
import java.io.InputStream;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * An advanced Generation Controller that parses the vocabulary from tokenizer.json.
 */
public class GenerationController {

    private final Map<String, Integer> vocab;
    private final Map<Integer, String> inverseVocab;
    private final int eosTokenId; // End-of-Sequence token

    public GenerationController(Context context) throws IOException, JSONException {
        // This now loads the much more complex tokenizer.json file.
        JSONObject tokenizerJson = loadJsonFromAssets(context, "tokenizer.json");
        
        // The vocabulary is typically nested inside the 'model' -> 'vocab' keys.
        this.vocab = parseVocabFromJson(tokenizerJson.getJSONObject("model").getJSONObject("vocab"));
        
        this.inverseVocab = new HashMap<>();
        for (Map.Entry<String, Integer> entry : vocab.entrySet()) {
            inverseVocab.put(entry.getValue(), entry.getKey());
        }

        // Find the End-of-Sequence token ID. This is crucial for stopping generation.
        int foundEosId = vocab.getOrDefault("<eos>", -1); // Common for Gemma
        if (foundEosId == -1) {
            // Fallback for other models if "<eos>" is not found.
            foundEosId = vocab.getOrDefault("</s>", -1);
        }
        this.eosTokenId = foundEosId;
    }

    private JSONObject loadJsonFromAssets(Context context, String fileName) throws IOException, JSONException {
        InputStream is = context.getAssets().open(fileName);
        int size = is.available();
        byte[] buffer = new byte[size];
        is.read(buffer);
        is.close();
        String json = new String(buffer, StandardCharsets.UTF_8);
        return new JSONObject(json);
    }

    private Map<String, Integer> parseVocabFromJson(JSONObject vocabJson) throws JSONException {
        Map<String, Integer> map = new HashMap<>();
        for (java.util.Iterator<String> it = vocabJson.keys(); it.hasNext(); ) {
            String key = it.next();
            map.put(key, vocabJson.getInt(key));
        }
        return map;
    }

    /**
     * A very basic tokenizer. A real implementation would use the BPE logic
     * also contained within tokenizer.json, but for now, this demonstrates vocab lookup.
     */
    public int[] tokenize(String text) {
        // NOTE: This is still a placeholder for a real BPE tokenizer.
        // A production app would use a proper Java BPE library that can fully parse tokenizer.json.
        String[] words = text.toLowerCase().trim().split("\\s+");
        List<Integer> tokenIds = new ArrayList<>();
        
        // Gemma uses a BOS (Beginning-of-Sequence) token.
        if (vocab.containsKey("<bos>")) {
            tokenIds.add(vocab.get("<bos>"));
        }

        for (String word : words) {
            tokenIds.add(vocab.getOrDefault(word, 2)); // 2 is typically the <unk> token
        }

        int[] result = new int[tokenIds.size()];
        for (int i = 0; i < tokenIds.size(); i++) {
            result[i] = tokenIds.get(i);
        }
        return result;
    }

    /**
     * Converts a single token ID back to its string representation.
     */
    public String detokenize(int tokenId) {
        // Special handling for the <bos> token which shouldn't be displayed.
        if (vocab.containsKey("<bos>") && tokenId == vocab.get("<bos>")) {
            return "";
        }
        return inverseVocab.getOrDefault(tokenId, "");
    }

    /**
     * The main generation loop.
     */
    public void generate(NativeEngine engine, long handle, int[] initialTokenIds, int maxNewTokens, java.util.function.Consumer<String> onTokenGenerated) {
        int[] currentTokens = initialTokenIds;

        // The first call is the "prefill" phase with the full prompt.
        int[] logits = engine.nativeGenerate(handle, currentTokens);
        int nextTokenId = argmax(logits);

        // The "decoding" loop, feeding one token back in at a time.
        for (int i = 0; i < maxNewTokens; i++) {
            if (nextTokenId == eosTokenId) {
                break;
            }

            String nextTokenStr = detokenize(nextTokenId);
            onTokenGenerated.accept(nextTokenStr);

            // Prepare for the next iteration.
            currentTokens = new int[]{nextTokenId};
            logits = engine.nativeGenerate(handle, currentTokens);
            nextTokenId = argmax(logits);
        }
    }

    private int argmax(int[] logits) {
        if (logits == null || logits.length == 0) {
            return eosTokenId; // Default to stopping if there's an error
        }
        int maxIndex = 0;
        for (int i = 1; i < logits.length; i++) {
            if (logits[i] > logits[maxIndex]) {
                maxIndex = i;
            }
        }
        return maxIndex;
    }
}
