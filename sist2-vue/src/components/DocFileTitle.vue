<template>
  <a :href="`f/${doc._source.index}/${doc._id}`" class="file-title-anchor" target="_blank">
    <div class="file-title" :title="doc._source.path + '/' + doc._source.name + ext(doc)"
         v-html="fileName() + ext(doc)"></div>
  </a>
</template>

<script>
import {ext} from "@/util";

export default {
  name: "DocFileTitle",
  props: ["doc"],
  methods: {
    ext: ext,
    fileName() {
      if (!this.doc.highlight) {
        return this.doc._source.name;
      }
      if (this.doc.highlight["name.nGram"]) {
        return this.doc.highlight["name.nGram"];
      }
      if (this.doc.highlight.name) {
        return this.doc.highlight.name;
      }
      return this.doc._source.name;
    }
  }
}
</script>

<style scoped>
.file-title-anchor {
  max-width: calc(100% - 1.2rem);
}

.file-title {
  width: 100%;
  line-height: 1rem;
  height: 1.1rem;
  white-space: nowrap;
  text-overflow: ellipsis;
  overflow: hidden;
  font-size: 16px;
  font-family: "Source Sans Pro", sans-serif;
  font-weight: bold;
}

.theme-black .file-title {
  color: #ddd;
}
.theme-black .file-title:hover {
  color: #fff;
}

.theme-light .file-title {
  color: black;
}

.doc-card .file-title {
  font-size: 12px;
}
</style>