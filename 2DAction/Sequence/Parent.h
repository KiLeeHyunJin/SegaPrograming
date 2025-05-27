#pragma once



namespace GameLib
{

    namespace Sequence
    {

        class GameOver;
        class GameParent;
        class Title;
        class Ending;

        class Parent
        {
        public:

            enum eSequenc
            {
                Seq_GameOver,
                Seq_Game,
                Seq_Title,
                Seq_Ending,
                Seq_None,
            };

#pragma region SingleTon
            static Parent* instance()
            {
                if (mInstance == nullptr)
                {
                    mInstance = new Parent();
                }
                return mInstance;
            }

            static bool create()
            {
                if (mInstance == nullptr)
                {
                    mInstance = new Parent();
                    return true;
                }
                return false;
            }
            static void release()
            {
                if (mInstance != nullptr)
                {
                    delete mInstance;
                }
            }
#pragma endregion


            void Update();


        private:
            Parent() {}
            ~Parent() {}

            Ending*     m_pEnding       = 0;
            Title*      m_pTitle        = 0;
            GameParent* m_pGameParent   = 0;
            GameOver*   m_pGameOver     = 0;


            static Parent* mInstance;
        };

    }
}

