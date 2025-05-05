<script setup>
import { RouterView } from 'vue-router'
import { ref, onMounted, watch } from 'vue'
import { useTraceStore } from '@/stores/trace'
import SplashView from '@/views/SplashView.vue'
import router from '@/router'

const traceStore = useTraceStore()
const showSplash = ref(true)

// 直接监听traceData变化
watch(() => traceStore.traceData, (newVal) => {
  if (newVal) {
    showSplash.value = false
    router.push('/flame')
    console.log('show splash is false')
  }
}, { immediate: true })
</script>

<template>
  <div v-if="showSplash" class="splash-container">
    <SplashView/>
  </div>
  <div v-else class="app-container">
    <nav class="sidebar">
      <h1 class="sidebar-title">Trace Viewer</h1>
      <router-link to="/flame">火焰图</router-link>
      <router-link to="/tags">标签统计</router-link>
      <router-link to="/timeline">时序分析</router-link>
    </nav>
    <main class="main-content">
      <RouterView />
    </main>
  </div>
</template>

<style scoped>
.splash-container {
  display: flex;
  justify-content: center;
  align-items: center;
  height: 100vh;
  background: #ffffff;
}

.app-container {
  display: flex;
  min-height: 100vh;
}

.sidebar {
  position: fixed;
  left: 0;
  top: 0;
  bottom: 0;
  width: 240px;
  background: #2c3e50;
  color: white;
  padding: 20px;
  box-shadow: 2px 0 10px rgba(0, 0, 0, 0.1);
  transition: all 0.3s ease;
}

.sidebar-title {
  color: white;
  font-size: 1.5rem;
  font-weight: bold;
  margin-bottom: 20px;
  padding: 10px;
}

.sidebar a {
  color: white;
  text-decoration: none;
  display: block;
  padding: 10px;
  border-radius: 4px;
  transition: background-color 0.3s;
}

.sidebar a:hover {
  background-color: rgba(255, 255, 255, 0.1);
}

.sidebar a.router-link-active {
  background-color: rgba(255, 255, 255, 0.2);
}

.main-content {
  margin-left: 240px;
  flex: 1;
  padding: 20px;
}
</style>
