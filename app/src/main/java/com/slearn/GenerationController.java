package com.slearn;

import android.content.Context;
import org.json.JSONException;
import org.json.JSONObject;
import java.io.IOException;
import java.io.InputStream;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * A simplified BPE Tokenizer and Generation Controller.
 * This class is responsible for text processing and managing the inference loop.
 * In a production app, this would be a more robust library.
 */
public class GenerationController {

    private final Map<String, Integer> vocab;
    private final Map<Integer, String> inverseVocab;

    public GenerationController(Context context) throws IOException, JSONException {
        this.vocab = loadVocab(context, "vocab.json");
        this.inverseVocab = new HashMap<>();
        for (Map.Entry<String, Integer> entry : vocab.entrySet()) {
            inverseVocab.put(entry.getValue(), entry.getKey());
        }
    }

    private Map<String, Integer> loadVocab(Context context, String fileName) throws IOException, JSONException {
        InputStream is = context.getAssets().open(fileName);
        int size = is.available();
        byte[] buffer = new byte[size];
        is.read(buffer);
        is.close();
        String json = new String(buffer, StandardCharsets.UTF_8);
        JSONObject jsonObject = new JSONObject(json);
        Map<String, Integer> map = new HashMap<>();
        for (java.util.Iterator<String> it = jsonObject.keys(); it.hasNext(); ) {
            String key = it.next();
            map.put(key, jsonObject.getInt(key));
        }
        return map;
    }

    /**
     * A very basic tokenizer. It splits by space and looks up tokens.
     * A real BPE tokenizer is much more complex.
     */
    public int[] tokenize(String text) {
        // NOTE: This is a placeholder for a real BPE tokenizer.
        // It's a "dummy" tokenizer that splits by space for demonstration.
        String[] words = text.toLowerCase().trim().split("\\s+");
        List<Integer> tokenIds = new ArrayList<>();
        // Add a Beginning-of-Sequence (BOS) token if the vocab has one.
        if (vocab.containsKey("<s>")) {
            tokenIds.add(vocab.get("<s>"));
        }
        for (String word : words) {
            tokenIds.add(vocab.getOrDefault(word, 2)); // 2 is often the <unk> token
        }
        int[] result = new int[tokenIds.size()];
        for (int i = 0; i < tokenIds.size(); i++) {
            result[i] = tokenIds.get(i);
        }
        return result;
    }

    /**
     * Converts a single token ID back to a string.
     */
    public String detokenize(int tokenId) {
        return inverseVocab.getOrDefault(tokenId, "<unk>");
    }

    /**
     * The main generation loop.
     * @param engine The native C++ engine.
     * @param handle The conversation handle.
     * @param initialTokenIds The tokenized user prompt.
     * @param maxNewTokens The maximum number of tokens to generate.
     * @param onTokenGenerated A callback to stream generated tokens to the UI.
     */
    public void generate(NativeEngine engine, long handle, int[] initialTokenIds, int maxNewTokens, java.util.function.Consumer<String> onTokenGenerated) {
        int[] currentTokens = initialTokenIds;
        int eosTokenId = vocab.getOrDefault("</s>", -1); // End-of-Sequence token

        for (int i = 0; i < maxNewTokens; i++) {
            // 1. Call the C++ engine with the current tokens.
            int[] logits = engine.nativeGenerate(handle, currentTokens); // This JNI call returns an array of logits (as int[] for simplicity)

            // 2. Sample the next token from the logits (simple argmax).
            int nextTokenId = argmax(logits);

            // 3. Check for stopping condition.
            if (nextTokenId == eosTokenId) {
                break;
            }

            // 4. De-tokenize the generated token and send it to the UI.
            String nextTokenStr = detokenize(nextTokenId);
            onTokenGenerated.accept(nextTokenStr);

            // 5. Prepare for the next iteration. The next input is just the single new token.
            currentTokens = new int[]{nextTokenId};
        }
    }

    /**
     * A simple argmax sampler to find the most likely token from the logits.
     */
    private int argmax(int[] logits) {
        if (logits == null || logits.length == 0) {
            return -1;
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
