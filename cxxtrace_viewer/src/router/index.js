import { createRouter, createWebHashHistory } from 'vue-router'

const router = createRouter({
  history: createWebHashHistory(import.meta.env.BASE_URL),
  routes: [
    {
      path: '/',
      name: 'home',
      component: () => import('../views/SplashView.vue'),
    },
    {
      path: '/main',
      name: 'main',
      component: () => import('../App.vue'),
    },
    {
      path: '/flame',
      name: 'flamegraph',
      component: () => import('../views/FlameGraphView.vue'),
    },
    {
      path: '/tags',
      name: 'tags',
      component: () => import('../views/TagsView.vue'),
    },
    {
      path: '/timeline',
      name: 'timeline',
      component: () => import('../views/CallGraphView.vue'),
    },
  ],
})

export default router
