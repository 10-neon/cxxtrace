<script setup>
const emit = defineEmits(['file-selected'])

const handleFileChange = (event) => {
  const file = event.target.files[0]
  if (!file) return
  const reader = new FileReader()
  reader.onload = (e) => {
    try {
      const data = JSON.parse(e.target.result)
      emit('file-selected', data)
    } catch (error) {
      console.error('Error parsing JSON file:', error)
    }
  }
  reader.readAsText(file)

}
</script>

<template>
  <div class="file-selector">
    <input
      type="file"
      accept=".json"
      @change="handleFileChange"
      class="file-input"
      id="file-upload"
    />
    <label for="file-upload" class="upload-button">
      Open Trace
    </label>
  </div>
</template>

<style scoped>
.file-input {
  display: none;
}

.upload-button {
  display: inline-block;
  padding: 10px 20px;
  background-color: #409eff;
  color: white;
  border-radius: 4px;
  cursor: pointer;
  font-size: 16px;
}

.upload-button:hover {
  background-color: #66b1ff;
}
</style>
